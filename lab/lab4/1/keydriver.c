#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "address_map_arm.h"
#include "interrupt_ID.h"

#define KEY_MAJOR  0 //0으로 하여 우리가 써야 할 MAJOR NUM을 알아서 찾아내도록함
#define KEY_NAME   "KEY"
#define KEY_MODULE_VERSION "KEY v0.1"

void* LW_virtual;
static int KEY_major = 0;
volatile int* KEY_ptr;

int KEY_open(struct inode *minode, struct file *mfile){
    KEY_ptr = ioremap_nocache(LW_BRIDGE_BASE, LW_BRIDGE_SPAN) + KEY_BASE;
    printk(KERN_INFO "[KEY_file_open]\n");
    return 0;
}

int KEY_release(struct inode *minode, struct file *mfile){
    printk(KERN_INFO "[KEY_file_release]\n");
    return 0;
}

ssize_t KEY_read(struct file *inode, char *buffer, size_t length, loff_t *off_wat){
    printk(KERN_INFO "KEY READ : value: %d\n", *KEY_ptr);
    put_user(*KEY_ptr, buffer);  
    return length;
}

static struct file_operations KEY_fops = {
    .owner = THIS_MODULE,
    .read = KEY_read,
    .open  = KEY_open,
    .release = KEY_release,
};

int KEY_init(void){
    // major num part
    int result = register_chrdev(KEY_MAJOR, KEY_NAME, &KEY_fops);
    if(result < 0){
        printk(KERN_WARNING "Cant get any major\n");
        return result;
    }
    KEY_major = result;
    printk(KERN_INFO "[KEY_init] major number: %d \n", result);

    return 0;
}

void KEY_exit(void){
    printk(KERN_INFO "[KEY_exit]\n");

    unregister_chrdev(KEY_major, KEY_NAME);
}


module_init(KEY_init);
module_exit(KEY_exit);