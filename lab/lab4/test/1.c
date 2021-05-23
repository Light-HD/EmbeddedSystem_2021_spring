#include <linux/module.h>
#include <linux/kernel.h>

static int __init kau_module_init(void){
    printk(KERN_INFO "KAU module init...\n");
    return 0;
}

static int __exit kau_module_exit(void){
    printk(KERN_INFO "KAU module clean up...\n");
}

module_init(kau_module_init);
module_exit(kau_module_exit);