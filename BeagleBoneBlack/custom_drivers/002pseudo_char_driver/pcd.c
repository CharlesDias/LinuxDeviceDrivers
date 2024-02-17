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

/* Memory size of the pseudo device.*/
#define DEV_MEM_SIZE        (512)
char device_buffer[DEV_MEM_SIZE];

/* Device number holder.*/
dev_t device_number;

/* cdev structure holder.*/
struct cdev pcd_cdev;

/* Function to handle llseek system call for the pseudo device.*/
loff_t pcd_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t temp;

    pr_info("Lseek requested with offset = %lld and whence = %d\n", offset, whence);

    /* Handle the different cases of whence and adjust the file position accordingly.*/
    switch (whence)
    {
    case SEEK_SET:
        if (offset > DEV_MEM_SIZE || offset < 0)
        {
            return -EINVAL;  // Return error code for invalid argument
        }
        filp->f_pos = offset;
        break;
    case SEEK_CUR:
        temp = filp->f_pos + offset;
        if (temp > DEV_MEM_SIZE || temp < 0)
        {
            return -EINVAL;  // Return error code for invalid argument
        }
        filp->f_pos = temp;
        break;
    case SEEK_END:
        temp = DEV_MEM_SIZE + offset;
        if (temp > DEV_MEM_SIZE || temp < 0)
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
    pr_info("Read requested with count = %zu and f_pos = %lld\n", count, *f_pos);

    /* Adjust the count if trying to read beyond end of device.*/
    if ((*f_pos + count) > DEV_MEM_SIZE)
    {
        count = DEV_MEM_SIZE - *f_pos;
    }

    /* Copy data from device_buffer to user buffer.*/
    if (copy_to_user(buff, device_buffer + *f_pos, count))
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
    pr_info("Write requested with count = %zu and f_pos = %lld\n", count, *f_pos);

    /* Adjust the count if trying to write beyond end of device.*/
    if ((*f_pos + count) > DEV_MEM_SIZE)
    {
        count = DEV_MEM_SIZE - *f_pos;
    }

    if (!count)
    {
        pr_err("No space left on the device\n");
        return -ENOMEM;  // Return error code for no memory
    }

    /* Copy data from user buffer to device_buffer.*/
    if (copy_from_user(device_buffer + *f_pos, buff, count))
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
int pcd_open(struct inode *inode, struct file *file)
{
    pr_info("Open operation was successful\n");
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

/* Device class holder.*/
struct class *class_pcd;

/* Device structure holder.*/
struct device *device_pcd;

/* Function to initialize the driver module.*/
static int __init pcd_driver_init(void)
{
    int ret;

    pr_info("Pseudo Char Driver Initialized\n");

    /* 1. Dynamically allocate a device number.*/
    ret = alloc_chrdev_region(&device_number, 0, 1, "pcd_devices");
    if(ret < 0)
    {
        pr_err("Alloc chrdev failed\n");
        goto out;
    }
    pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

    /* 2. Create a device class visible in /sys/class/ */
    class_pcd = class_create(THIS_MODULE, PCD_CLASS);
    if(IS_ERR(class_pcd))
    {
        pr_err("Class creation failed\n");
        ret = PTR_ERR(class_pcd);
        goto unreg_chrdev;
    }

    /* 3. Initialize the cdev structure and link the file operations to it.*/
    cdev_init(&pcd_cdev, &pcd_fops);
    pcd_cdev.owner = THIS_MODULE;

    /* 4. Register a device to the Kernel VFS system.*/
    ret = cdev_add(&pcd_cdev, device_number, 1);
    if(ret < 0)
    {
        pr_err("Cdev add failed\n");
        goto class_del;
    }

    /* 5. Create a device and register it with sysfs.*/
    device_pcd = device_create(class_pcd, NULL, device_number, NULL, PCD_DEVICE_NAME);
    if(IS_ERR(device_pcd))
    {
        pr_err("Device creation failed\n");
        ret = PTR_ERR(device_pcd);
        goto cdev_del;
    }

    pr_info("Module init was successful.\n");
    return 0;

cdev_del:
    cdev_del(&pcd_cdev);
class_del:
    class_destroy(class_pcd);
unreg_chrdev:
    unregister_chrdev_region(device_number, 1);
out:
    pr_err("Module init failed.\n");
    return ret;
}

/* Function to clean up resources when the module is removed.*/
static void __exit pcd_driver_cleanup(void)
{
    /* 1. Destroy the device.*/
    device_destroy(class_pcd, device_number);

    /* 2. Unregister the cdev structure.*/
    cdev_del(&pcd_cdev);

    /* 3. Unregister the device class.*/
    class_destroy(class_pcd);

    /* 4. Unregister the device number.*/
    unregister_chrdev_region(device_number, 1);

    pr_info("Pseudo Char Driver unloaded\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charles Dias");
MODULE_DESCRIPTION("A pseudo character driver");
MODULE_INFO(board, "BeagleBone Black Revision A5");