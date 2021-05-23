#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "address_map_arm.h"
#include "interrupt_ID.h"

#define HEX30_MAJOR  0 //0으로 하여 우리가 써야 할 MAJOR NUM을 알아서 찾아내도록함
#define HEX30_NAME   "HEX30"
#define HEX30_MODULE_VERSION "HEX30 v0.1"

static int HEX30_major = 0;
volatile int* HEX30_ptr;

int HEX30_open(struct inode *minode, struct file *mfile){
    HEX30_ptr = ioremap_nocache(LW_BRIDGE_BASE, LW_BRIDGE_SPAN) + HEX3_HEX0_BASE;
    printk(KERN_INFO "[HEX30_file_oepn]\n");
    return 0;
}

int HEX30_release(struct inode *minode, struct file *mfile){
    printk(KERN_INFO "[HEX30_file_release]\n");
    return 0;
}

int num2hex(char num){
    int hexNum[] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 
                            0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111};
    if (num < 0)
        return (0b01000000 << 8) | (hexNum[-num]);

    else if (num >= 10)
        return (hexNum[num/10] << 8) | (hexNum[num%10]);
    
    else 
        return hexNum[num];
}
ssize_t HEX30_write_byte(struct file *inode, const char *gdata, size_t length, loff_t *off_wat){
    char c;

    get_user(c, gdata);
    *HEX30_ptr = num2hex(c);
    printk(KERN_INFO "HEX30_write_byte %d\n",c);
    return length;
}

static struct file_operations HEX30_fops = {
    .owner = THIS_MODULE,
    .write = HEX30_write_byte,
    .open  = HEX30_open,
    .release = HEX30_release,
};

int HEX30_init(void){
    int result = register_chrdev(HEX30_MAJOR, HEX30_NAME, &HEX30_fops);
    if(result < 0){
        printk(KERN_WARNING "Cant get any major\n");
        return result;
    }
    HEX30_major = result;
    printk(KERN_INFO "[HEX30_init] major number: %d \n", result);
    return 0;
}

void HEX30_exit(void){
    printk(KERN_INFO "[HEX30_exit]\n");
    unregister_chrdev(HEX30_major, HEX30_NAME);
}


module_init(HEX30_init);
module_exit(HEX30_exit);
