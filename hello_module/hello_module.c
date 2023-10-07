#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h>   /* Needed for the macros */

static int __init hello_start(void)
{
    printk(KERN_INFO "Loading hello module...\n");
    printk(KERN_INFO "Hello, World!\n");
    return 0;
}

static void __exit hello_end(void)
{
    printk(KERN_INFO "The module is now unloaded!\n");
    printk(KERN_INFO "Goodbye, World!\n");
}

module_init(hello_start);
module_exit(hello_end);

MODULE_AUTHOR("Charles Dias");
MODULE_DESCRIPTION("A Hello, World Module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");