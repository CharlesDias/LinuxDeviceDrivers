#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/kdev_t.h>
#include <linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

#define PCD_DEVICE_NAME     "pcd"
#define PCD_CLASS           "pcd_class"

#define NO_OF_DEVICES 4

#define MEM_SIZE_MAX_PCDEV1         1024
#define MEM_SIZE_MAX_PCDEV2         512
#define MEM_SIZE_MAX_PCDEV3         1024
#define MEM_SIZE_MAX_PCDEV4         512

/* Memory size of the pseudo device.*/
char device_buffer_pcdev1[MEM_SIZE_MAX_PCDEV1];
char device_buffer_pcdev2[MEM_SIZE_MAX_PCDEV2];
char device_buffer_pcdev3[MEM_SIZE_MAX_PCDEV3];
char device_buffer_pcdev4[MEM_SIZE_MAX_PCDEV4];

/*Device private data structure */

struct pcdev_private_data
{
   char *buffer;
   unsigned size;
   const char *serial_number;
   int perm;
   struct cdev cdev;
};

/*Driver private data structure */
struct pcdrv_private_data
{
   int total_devices;
   dev_t device_number;     /* This holds the device number */
   struct class *class_pcd;
   struct device *device_pcd;
   struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
};

struct pcdrv_private_data pcdrv_data =
{
   .pcdev_data = {
      [0] = {.buffer = device_buffer_pcdev1, .size = MEM_SIZE_MAX_PCDEV1, .serial_number = "PCDEV1XYZ123", .perm = O_RDONLY},
      [1] = {.buffer = device_buffer_pcdev2, .size = MEM_SIZE_MAX_PCDEV2, .serial_number = "PCDEV2XYZ123", .perm = O_WRONLY},
      [2] = {.buffer = device_buffer_pcdev3, .size = MEM_SIZE_MAX_PCDEV3, .serial_number = "PCDEV3XYZ123", .perm = O_RDWR},
      [3] = {.buffer = device_buffer_pcdev4, .size = MEM_SIZE_MAX_PCDEV4, .serial_number = "PCDEV4XYZ123", .perm = O_RDWR}}
};

/* Function to handle llseek system call for the pseudo device.*/
loff_t pcd_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t temp;
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;
    int max_size = pcdev_data->size;

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
    int max_size = pcdev_data->size;

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
    int max_size = pcdev_data->size;

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


int check_permission(int dev_perm, int acc_mode)
{
    /* Ensures read and write access */
    if(dev_perm == O_RDWR)
    {
        return 0;
    }

    /* Ensures readonly access */
    if((dev_perm == O_RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
    {
        return 0;
    }

    /* Ensures writeonly access */
    if((dev_perm == O_WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ)))
    {
        return 0;
    }

    return -EPERM;
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
    ret = check_permission(pcdev_data->perm, filp->f_mode);

    (!ret) ? pr_info("Open operation was successful\n") : pr_err("Open operation was unsuccessful\n");

    return 0;
}

/* Function to handle release system call for the pseudo device.*/
int pcd_release(struct inode *inode, struct file *filp)
{
   pr_info("Close operation was successful\n");
   return 0;
}

/* File operations structure that links the system calls to the functions defined above.*/
struct file_operations pcd_fops = {
    .open = pcd_open,
    .release = pcd_release,
    .read = pcd_read,
    .write = pcd_write,
    .llseek = pcd_llseek,
    .owner = THIS_MODULE
};

/* Function to initialize the driver module.*/
static int __init pcd_driver_init(void)
{
    int ret;
    int i;

    pr_info("Pseudo Char Driver Initialized\n");

    /* 1. Dynamically allocate a device number.*/
    ret = alloc_chrdev_region(&pcdrv_data.device_number, 0, NO_OF_DEVICES, "pcd_devices");
    if(ret < 0)
    {
        pr_err("Alloc chrdev failed\n");
        goto out;
    }

    for(i = 0; i < NO_OF_DEVICES; i++)
    {
        pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(pcdrv_data.device_number + i), MINOR(pcdrv_data.device_number + i));
    }

    /* 2. Create a device class visible in /sys/class/ */
    pcdrv_data.class_pcd = class_create(THIS_MODULE, PCD_CLASS);
    if(IS_ERR(pcdrv_data.class_pcd))
    {
        pr_err("Class creation failed\n");
        ret = PTR_ERR(pcdrv_data.class_pcd);
        goto unreg_chrdev;
    }

    for(i = 0; i < NO_OF_DEVICES; i++)
    {
        /* 3. Initialize the cdev structure and link the file operations to it.*/
        cdev_init(&pcdrv_data.pcdev_data[i].cdev, &pcd_fops);
        pcdrv_data.pcdev_data[i].cdev.owner = THIS_MODULE;

        /* 4. Register a device to the Kernel VFS system.*/
        ret = cdev_add(&pcdrv_data.pcdev_data[i].cdev, pcdrv_data.device_number + i, 1);
        if(ret < 0)
        {
            pr_err("Cdev add failed\n");
            goto cdev_del;
        }

        /* 5. Create a device and register it with sysfs.*/
        pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number + i, NULL, "pcdev-%d", i + 1);
        if(IS_ERR(pcdrv_data.device_pcd))
        {
            pr_err("Device creation failed\n");
            ret = PTR_ERR(pcdrv_data.device_pcd);
            goto class_del;
        }
    }

    pr_info("Module init was successful.\n");
    return 0;

cdev_del:
class_del:
    for(i = 0; i < NO_OF_DEVICES; i++)
    {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number + i);
        cdev_del(&pcdrv_data.pcdev_data[i].cdev);
    }
    class_destroy(pcdrv_data.class_pcd);
unreg_chrdev:
    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);
out:
    pr_err("Module init failed.\n");
    return ret;
}

/* Function to clean up resources when the module is removed.*/
static void __exit pcd_driver_cleanup(void)
{
    int i;

    for(i = 0; i < NO_OF_DEVICES; i++)
    {
        /* 1. Destroy the device. It will remove a device node from /dev */
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number + i);

        /* 2. Delete the cdev structure. It will make the system forget about this character device */
        cdev_del(&pcdrv_data.pcdev_data[i].cdev);
    }

    /* 3. Unregister the device class.*/
    class_destroy(pcdrv_data.class_pcd);

    /* 4. Unregister the device number.*/
    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);

    pr_info("Pseudo Char Driver unloaded\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charles Dias");
MODULE_DESCRIPTION("A pseudo character driver which handles N devices.");
MODULE_INFO(board, "BeagleBone Black Revision A5");