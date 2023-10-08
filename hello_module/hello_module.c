#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h>   /* Needed for the __init and __exit macros */

static int __init hello_init(void)
{
    printk(KERN_INFO "Loading hello module...\n");
    pr_info("Hello, World!\n"); // It is the same as printk(KERN_INFO "Hello, World!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("The module is now unloaded!\n"); // It is the same as printk(KERN_INFO "The module is now unloaded!\n");
    printk(KERN_INFO "Goodbye, World!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Charles Dias <charlesdias.cd@outlook.com>");
MODULE_DESCRIPTION("A Hello, World Module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");