#define pr_fmt(fmt) "%s:%s: " fmt, KBUILD_MODNAME, __func__

#include <linux/module.h>  // Core header for loading LKMs
#include <linux/kernel.h>  // Contains types, macros, functions for the kernel
#include <linux/fs.h>      // Header for the Linux file system support
#include <linux/uaccess.h> // Contains the macro for copy_(from|to)_user
#include <linux/cdev.h>    // Header for character devices

#define DEVICE_NAME "examplechar"
#define CLASS_NAME "example"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charles Dias");
MODULE_DESCRIPTION("A simple Linux char driver with dynamic registration and error handling");
MODULE_VERSION("0.1");

static dev_t dev_num;            // Contains major and minor number
static struct cdev example_cdev; // Character device structure
static struct class *example_char_class = NULL;

static char message[256] = {0};
static short size_of_message;

static char* example_devnode(const struct device *dev, umode_t *mode);

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
    .owner = THIS_MODULE,
};

static int __init example_char_init(void)
{
    static struct device *example_char_device = NULL;

    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0)
    {
        pr_err("Failed to allocate a major number\n");
        return -1;
    }

    pr_info("Major = %d Minor = %d \n", MAJOR(dev_num), MINOR(dev_num));

    cdev_init(&example_cdev, &fops);

    if (cdev_add(&example_cdev, dev_num, 1) == -1)
    {
        pr_err("Error adding cdev\n");
        goto unregister_region;
    }

    example_char_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(example_char_class))
    {
        pr_err("Failed to register device class\n");
        goto del_cdev;
    }

    // Set the devnode callback
    example_char_class->devnode = example_devnode;

    example_char_device = device_create(example_char_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(example_char_device))
    {
        pr_err("Failed to create the device\n");
        goto destroy_class;
    }

    pr_info("ExampleChar module loaded\n");
    return 0;

destroy_class:
    class_destroy(example_char_class);

del_cdev:
    cdev_del(&example_cdev);

unregister_region:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

static void __exit example_char_exit(void)
{
    device_destroy(example_char_class, dev_num);
    class_destroy(example_char_class);
    cdev_del(&example_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("ExampleChar module unloaded\n");
}

//char *(*devnode)(const struct device *dev, umode_t *mode);
static char* example_devnode(const struct device *dev, umode_t *mode)
{
    if (mode) {
        // Set the permissions, e.g., 0666 for universal read/write
        *mode = 0666;
    }
    return NULL; // return NULL means the default name will be used for the device
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    pr_info("ExampleChar device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int bytes_to_read;

    // Check if trying to read beyond our message
    if (*offset >= size_of_message)
        return 0; // end-of-file

    // Determine bytes to read: minimum of remaining bytes or len
    bytes_to_read = min((size_t)size_of_message - (size_t)*offset, len);

    if (copy_to_user(buffer, message + *offset, bytes_to_read))
    {
        pr_err("Failed to send bytes to the user\n");
        return -EFAULT;
    }

    *offset += bytes_to_read; // Update offset
    pr_info("Sent %d characters to the user\n", bytes_to_read);

    return bytes_to_read;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    if (len > sizeof(message) - 1)
    {
        pr_err("Input too long. Max length is %zu\n", sizeof(message) - 1);
        return -EFAULT;
    }

    if (copy_from_user(message, buffer, len))
    {
        pr_err("Failed to receive bytes from the user\n");
        return -EFAULT;
    }

    message[len] = '\0'; // Null-terminate
    size_of_message = len;
    pr_info("Received %zu characters from the user\n", len);

    return len;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("ExampleChar device closed\n");
    return 0;
}

module_init(example_char_init);
module_exit(example_char_exit);
