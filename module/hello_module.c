#include <linux/kernel.h>
#include <linux/module.h>

int init_module(void)
{
    printk(KERN_EMERGE "HELLO MODULE IM IN KERNEL\n");
    return 0;
}

void cleanup_module(void){
    printk("<0>Bye module~~\n");
}

module_init(init_module);
module_exit(cleanup_module);

MODULE_LICENSE("GPL");