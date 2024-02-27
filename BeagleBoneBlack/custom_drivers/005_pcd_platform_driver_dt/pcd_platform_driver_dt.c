#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include<linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/of_device.h>

#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__

#define MAX_DEVICES 10

#define PCD_CLASS "pcd_class"

/* Enumeration of device names */
enum pcdev_names
{
    PCDEVA1X,
    PCDEVB1X,
    PCDEVC1X,
    PCDEVD1X
};

struct device_config 
{
    int config_item1;
    int config_item2;
};

/* Configuration data of the driver for devices */
struct device_config pcdev_config[] = 
{
    [PCDEVA1X] = {.config_item1 = 60, .config_item2 = 21},
    [PCDEVB1X] = {.config_item1 = 50, .config_item2 = 22},
    [PCDEVC1X] = {.config_item1 = 40, .config_item2 = 23},
    [PCDEVD1X] = {.config_item1 = 30, .config_item2 = 24}
};

/*Device private data structure */
struct pcdev_private_data
{
    struct pcdev_platform_data pdata;
    char *buffer;
    dev_t dev_num;
    struct cdev cdev;
};

/*Driver private data structure */
struct pcdrv_private_data
{
    int total_devices;
    dev_t device_num_base; /* This holds the device number */
    struct class *class_pcd;
    struct device *device_pcd;
};

/* Driver's private data */
struct pcdrv_private_data pcdrv_data;

int check_permission(int dev_perm, int acc_mode)
{
    /* Ensures read and write access */
    if (dev_perm == O_RDWR)
    {
        return 0;
    }

    /* Ensures readonly access */
    if ((dev_perm == O_RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
    {
        return 0;
    }

    /* Ensures writeonly access */
    if ((dev_perm == O_WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ)))
    {
        return 0;
    }

    return -EPERM;
}

/* Function to handle llseek system call for the pseudo device.*/
loff_t pcd_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t temp;
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;
    int max_size = pcdev_data->pdata.size;

    pr_info("Lseek requested with offset = %lld and whence = %d\n", offset, whence);

    /* Handle the different cases of whence and adjust the file position accordingly.*/
    switch (whence)
    {
    case SEEK_SET:
        if (offset > max_size || offset < 0)
        {
            return -EINVAL;  // Return error code for invalid argument
        }
        filp->f_pos = offset;
        break;
    case SEEK_CUR:
        temp = filp->f_pos + offset;
        if (temp > max_size || temp < 0)
        {
            return -EINVAL;  // Return error code for invalid argument
        }
        filp->f_pos = temp;
        break;
    case SEEK_END:
        temp = max_size + offset;
        if (temp > max_size || temp < 0)
        {
            return -EINVAL;  // Return error code for invalid argument
        }
        filp->f_pos = temp;
        break;
    default:
        return -EINVAL;  // Return error code for invalid argument
    }

    pr_info("New value of f_pos = %lld\n", filp->f_pos);

    /* Return updated file position value. */
    return filp->f_pos;
}

/* Function to handle read system call for the pseudo device.*/
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;
    int max_size = pcdev_data->pdata.size;

    pr_info("Read requested with count = %zu and f_pos = %lld\n", count, *f_pos);

    /* Adjust the count if trying to read beyond end of device.*/
    if ((*f_pos + count) > max_size)
    {
        count = max_size - *f_pos;
    }

    /* Copy data from device_buffer to user buffer.*/
    if (copy_to_user(buff, pcdev_data->buffer + *f_pos, count))
    {
        pr_err("Failed to copy data to user\n");
        return -EFAULT;  // Return error code for bad address
    }

    /* Update the file position.*/
    *f_pos += count;

    pr_info("Number of bytes successfully read = %zu and updated f_pos = %lld\n", count, *f_pos);

    /* Return number of bytes which have been successfully read. */
    return count;
}

/* Function to handle write system call for the pseudo device.*/
ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;
    int max_size = pcdev_data->pdata.size;

    pr_info("Write requested with count = %zu and f_pos = %lld\n", count, *f_pos);

    /* Adjust the count if trying to write beyond end of device.*/
    if ((*f_pos + count) > max_size)
    {
        count = max_size - *f_pos;
    }

    if (!count)
    {
        pr_err("No space left on the device\n");
        return -ENOMEM;  // Return error code for no memory
    }

    /* Copy data from user buffer to device_buffer.*/
    if (copy_from_user(pcdev_data->buffer + *f_pos, buff, count))
    {
        pr_err("Failed to copy data from user\n");
        return -EFAULT;  // Return error code for bad address
    }

    /* Update the file position.*/
    *f_pos += count;

    pr_info("Number of bytes successfully written = %zu and updated f_pos = %lld\n", count, *f_pos);

    /* Return number of bytes which have been successfully written. */
    return count;
}

/* Function to handle open system call for the pseudo device.*/
int pcd_open(struct inode *inode, struct file *filp)
{
    int ret;
    int minor_n;
    struct pcdev_private_data *pcdev_data;

    /* Get the minor number of the device which is being opened.*/
    minor_n = MINOR(inode->i_rdev);
    pr_info("Open operation requested for minor number %d\n", minor_n);

    /* Get the device's private data structure.*/
    pcdev_data = container_of(inode->i_cdev, struct pcdev_private_data, cdev);

    /* Save the device's private data structure in the file's private_data.*/
    filp->private_data = pcdev_data;

    /* Check if device is open for read or write or both.*/
    ret = check_permission(pcdev_data->pdata.perm, filp->f_mode);

    (!ret) ? pr_info("Open operation was successful\n") : pr_err("Open operation was unsuccessful\n");

    return ret;
}


/* Function to handle release system call for the pseudo device.*/
int pcd_release(struct inode *inode, struct file *filp)
{
    pr_info("Release operation was successful\n");
    return 0;
}

/* File operations structure that links the system calls to the functions defined above.*/
struct file_operations pcd_fops = {
    .open = pcd_open,
    .release = pcd_release,
    .read = pcd_read,
    .write = pcd_write,
    .llseek = pcd_llseek,
    .owner = THIS_MODULE};

/* Function to handle the removal of a platform device.*/
int pcd_platform_driver_remove(struct platform_device *pdev)
{
    struct pcdev_private_data  *dev_data = dev_get_drvdata(&pdev->dev);

    /* 1. Remove a device node from /sys/class */
    device_destroy(pcdrv_data.class_pcd, dev_data->dev_num);

    /* 2. Remove a cdev entry from /dev/ */
    cdev_del(&dev_data->cdev);

    /* 3. Update the total number of devices */
    pcdrv_data.total_devices--;

    dev_info(&pdev->dev, "A device is removed\n");
    return 0;
}

struct pcdev_platform_data* pcdev_get_platform_from_dt(struct device *dev)
{
    struct device_node *dev_node = dev->of_node;
    struct pcdev_platform_data *pdata;

    if(!dev_node)
    {
        dev_err(dev, "Device node not found\n");
        return NULL;
    }

    pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
    if(!pdata){
        dev_err(dev, "Failed to allocate memory\n");
        return ERR_PTR(-ENOMEM);
    }

    if(of_property_read_string(dev_node, "org,device-serial-num", &pdata->serial_number)){
        dev_err(dev, "Missing serial number property\n");
        return ERR_PTR(-EINVAL);
    }

    if(of_property_read_u32(dev_node, "org,size", &pdata->size)){
        dev_err(dev, "Missing size property\n");
        return ERR_PTR(-EINVAL);
    }

    if(of_property_read_u32(dev_node, "org,perm", &pdata->perm)){
        dev_err(dev, "Missing permission property\n");
        return ERR_PTR(-EINVAL);
    }

    return pdata;
}

struct of_device_id org_pcdev_dt_match[];

/* Function to handle the probe of a platform device.*/
int pcd_platform_driver_probe(struct platform_device *pdev)
{
    int ret;
    struct pcdev_private_data *dev_data;
    struct pcdev_platform_data *pdata;
    struct device *dev = &pdev->dev;
    int driver_data;

    /* Used to store matched entry of 'of_device_id' list of this driver */
    const struct of_device_id *match;

    dev_info(dev, "A device is detected\n");

    /* match will always be NULL if the LINUX doesn't support device tree, i.e. CONFIG_OF is off */
    match = of_match_device(of_match_ptr(org_pcdev_dt_match), dev);
    
    if(match)
    {
        /* 1. Get the platform data of the device tree.*/
        pdata = pcdev_get_platform_from_dt(dev);

        if(IS_ERR(pdata)){
            ret = PTR_ERR(pdata);
            dev_err(dev, "Failed to get platform data from device tree\n");
            return ret;
        }

        driver_data = (int)match->data;
    }
    else
    {
        dev_info(dev, "Device tree match not found\n");

        /* 1. Get the platform data of the device.*/
        pdata = (struct pcdev_platform_data *)dev_get_platdata(dev);
        driver_data = pdev->id_entry->driver_data;
    }

    if (!pdata)
    {
        dev_err(dev, "No platform data available\n");
        return -EINVAL;
    }

    /* 2. Dynamically allocate memory for the device private data.*/
    dev_data = devm_kzalloc(dev, sizeof(*dev_data), GFP_KERNEL);
    if (!dev_data)
    {
        dev_err(dev, "Failed to allocate memory for private data\n");
        return -ENOMEM;
    }

    /* 3. Save the platform data in the device private data structure.*/
    dev_set_drvdata(dev,dev_data);

    dev_data->pdata.size = pdata->size;
    dev_data->pdata.perm = pdata->perm;
    dev_data->pdata.serial_number = pdata->serial_number;

    dev_info(dev, "Device serial number = %s\n",dev_data->pdata.serial_number);
    dev_info(dev, "Device size = %d\n", dev_data->pdata.size);
    dev_info(dev, "Device permission = %d\n",dev_data->pdata.perm);

    dev_info(dev, "Config item 1 = %d\n",pcdev_config[driver_data].config_item1 );
    dev_info(dev, "Config item 2 = %d\n",pcdev_config[driver_data].config_item2 );

    /* 4. Dynamically allocate memory for the device buffer using size information from the platform data.*/
    dev_data->buffer = devm_kzalloc(dev, dev_data->pdata.size, GFP_KERNEL);
    if(!dev_data->buffer){
        dev_err(dev, "Failed to allocate memory for buffer\n");
        return -ENOMEM;
    }

    /* 5. Get the device number.*/
    dev_data->dev_num = pcdrv_data.device_num_base + pcdrv_data.total_devices;

    /* 6. Do cdev init and cdev add.*/
    cdev_init(&dev_data->cdev,&pcd_fops);
    dev_data->cdev.owner = THIS_MODULE;

    ret = cdev_add(&dev_data->cdev, dev_data->dev_num, 1);
    if(ret < 0){
        dev_err(dev, "Failed to add cdev\n");
        return ret;
    }

    /* 7. Create a device file in /dev and a device node in /sys/class.*/
    pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, dev, dev_data->dev_num, NULL, 
                                                            "pcdev-%d", pcdrv_data.total_devices);
    if(IS_ERR(pcdrv_data.device_pcd)){
        dev_err(dev, "Failed to create device file\n");
        ret = PTR_ERR(pcdrv_data.device_pcd);
        cdev_del(&dev_data->cdev);
        return ret;
    }

    dev_info(dev, "A device is created with device number = %d\n", pcdrv_data.total_devices);

    /* 8. Update the total number of devices */
    pcdrv_data.total_devices++;

    return 0;
}

struct platform_device_id pcdevs_ids[] = {
    [0] = {.name = "pcdev-A1x", .driver_data = PCDEVA1X},
    [1] = {.name = "pcdev-B1x", .driver_data = PCDEVB1X},
    [2] = {.name = "pcdev-C1x", .driver_data = PCDEVC1X},
    [3] = {.name = "pcdev-D1x", .driver_data = PCDEVD1X},
    { } /*Null termination */
};

/* Device table used for matching the device id with the driver via devicetree file */
struct of_device_id org_pcdev_dt_match[] = {
    {.compatible = "pcdev-A1x", .data = (void *)PCDEVA1X},
    {.compatible = "pcdev-B1x", .data = (void *)PCDEVB1X},
    {.compatible = "pcdev-C1x", .data = (void *)PCDEVC1X},
    {.compatible = "pcdev-D1x", .data = (void *)PCDEVD1X},
    { } /*Null termination */
};

struct platform_driver pcd_platform_driver = {
    .probe = pcd_platform_driver_probe,
    .remove = pcd_platform_driver_remove,
    .id_table = pcdevs_ids, /* Used to match the device id with the driver */
    .driver = {
        .name = "pseudo-char-device",
        .of_match_table = of_match_ptr(org_pcdev_dt_match) /* Used to match the device id with the driver via devicetree file */
    }
};

/* Function to initialize the driver module.*/
static int __init pcd_platform_driver_init(void)
{
    int ret;

    /* 1. Dynamically allocate a device number for MAX_DEVICES */
    ret = alloc_chrdev_region(&pcdrv_data.device_num_base, 0, MAX_DEVICES, "pcdevs");
    if (ret < 0)
    {
        pr_err("Alloc chrdev failed\n");
        return ret;
    }

    /* 2. Create a device class under /sys/class */
    pcdrv_data.class_pcd = class_create(THIS_MODULE, PCD_CLASS);
    if (IS_ERR(pcdrv_data.class_pcd))
    {
        pr_err("Class creation failed\n");
        ret = PTR_ERR(pcdrv_data.class_pcd);
        unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);
        return ret;
    }

    /* 3. Register a platform driver */
    ret = platform_driver_register(&pcd_platform_driver);
    if(ret < 0)
    {
        pr_err("Platform driver registration failed\n");
        class_destroy(pcdrv_data.class_pcd);
        unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);
        return ret;
    }

    pr_info("Platform driver loaded\n");

    return 0;
}

/* Function to clean up resources when the module is removed.*/
static void __exit pcd_platform_driver_cleanup(void)
{
    /* 1. Unregister the platform driver */
    platform_driver_unregister(&pcd_platform_driver);

    /* 2. Class destroy */
    class_destroy(pcdrv_data.class_pcd);

    /* 3. Unregister device numbers for MAX_DEVICES */
    unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);

    pr_info("pcd platform driver unloaded\n");
}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charles Dias");
MODULE_DESCRIPTION("A pseudo character platform driver which handles n platform pcdevs");
MODULE_INFO(board, "BeagleBone Black Revision A5");