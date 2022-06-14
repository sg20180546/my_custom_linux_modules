#include <linux/kernel.h>
#include <linux/module.h>

int init_module(void)
{
    printk(KERN_EMERGE "HELLO MODULE IM IN KERNEL");
    return 0;
}

void cleanup_module(void){
    printk("<0> Bye module~~\n");
}


MODULE_LICENSE("GPL");