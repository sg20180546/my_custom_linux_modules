#include <linux/kernel.h>
#include <linux/module.h>

int init_custom_module(void)
{
    printk(KERN_EMERG "HELLO MODULE IM IN KERNEL\n");
    return 0;
}

void cleanup_custom_module(void){
    printk("<0>Bye module~~\n");
}

module_init(init_custom_module);
module_exit(cleanup_custom_module);

MODULE_LICENSE("GPL");