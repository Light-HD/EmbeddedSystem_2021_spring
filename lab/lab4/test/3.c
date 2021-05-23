#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "address_map_arm.h"
#include "interrupt_ID.h"

#define LEDR_MAJOR  0
#define LEDR_NAME   "LEDR"
#define LEDR_MODULE_VERSION "LEDR v0.1"

static int LEDR_major = 0;
volatile int* LEDR_ptr;

int LEDR_open(struct inode *minode, struct file *mfile){
    LEDR_ptr = ioremap_nocache(LW_BRIDGE_BASE, LW_BRIDGE_SPAN) + LEDR_BASE;
    printk(KERN_INFO "[LEDR_oepn]\n");
    return 0;
}

int LEDR_release(struct inode *minode, struct file *mfile){
    printk(KERN_INFO "[LEDR_release]\n");
    return 0;
}

ssize_t LEDR_write_byte(struct file *inode, const char *gdata, size_t length, loff_t *off_wat){
    unsigned char c;
    get_user(c, gdata);
    *LEDR_ptr = c;
    printk(KERN_INFO "LEDR_write_byte %d\n",c);
    return length;
}

static struct file_operations LEDR_fops = {
    .owner = THIS_MODULE,
    .write = LEDR_write_byte,
    .open  = LEDR_open,
    .release = LEDR_release,
};

int LEDR_init(void){
    int result = register_chrdev(LEDR_MAJOR, LEDR_NAME, &LEDR_fops);
    if(result < 0){
        printk(KERN_WARNING "Cant get any major\n");
        return result;
    }
    LEDR_major = result;
    printk(KERN_INFO "[LEDR_init] major number: %d \n", result);
    return 0;
}

void LEDR_exit(void){
    printk(KERN_INFO "[LEDR_exit]\n");
    unregister_chrdev(LEDR_major, LEDR_NAME);
}


module_init(LEDR_init);
module_exit(LEDR_exit);