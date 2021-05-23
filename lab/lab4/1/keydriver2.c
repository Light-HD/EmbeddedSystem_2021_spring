#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "address_map_arm.h"
#include "interrupt_ID.h"

#define KEY_MAJOR  0 //0으로 하여 우리가 써야 할 MAJOR NUM을 알아서 찾아내도록 함
#define KEY_NAME   "KEY"
#define KEY_MODULE_VERSION "KEY v0.1"

void* LW_virtual;
static int KEY_major = 0;
volatile int* LEDR_ptr;
volatile int* KEY_ptr;
static int updown = 0;

irq_handler_t irq_handler(int irq, void *dev_id, struct pt_regs *regs){
    updown = !updown;
    *(KEY_ptr+3) = 0x1; //엣지캡쳐 제거
    
    return (irq_handler_t) IRQ_HANDLED;
}

int KEY_open(struct inode *minode, struct file *mfile){
    printk(KERN_INFO "[KEY_file_open]\n");
    return 0;
}

int KEY_release(struct inode *minode, struct file *mfile){
    printk(KERN_INFO "[KEY_file_release]\n");
    return 0;
}

ssize_t updown_read(struct file *inode, char *buffer, size_t length, loff_t *off_wat){
    printk(KERN_INFO "KEY READ : updown: %d\n", updown);
    put_user(updown, buffer);  
    return length;
}

static struct file_operations KEY_fops = {
    .owner = THIS_MODULE,
    .read = updown_read,
    .open  = KEY_open,
    .release = KEY_release,
};

static int __init initialize_pushbutton_handler(void){
    int value;
    // major num part
    int result = register_chrdev(KEY_MAJOR, KEY_NAME, &KEY_fops);
    if(result < 0){
        printk(KERN_WARNING "Cant get any major\n");
        return result;
    }
    KEY_major = result;
    printk(KERN_INFO "[KEY_init] major number: %d \n", result);

    // interrupt part
    
    LW_virtual = ioremap_nocache(LW_BRIDGE_BASE, LW_BRIDGE_SPAN); //주소값 받아오기
    LEDR_ptr = LW_virtual + LEDR_BASE;
    *LEDR_ptr = 0;

    KEY_ptr = LW_virtual + KEY_BASE;
    *(KEY_ptr+3) = 1; //엣지캡쳐 제거
    *(KEY_ptr+2) = 1; //IRQ 셋팅

    printk(KERN_INFO "[KEY INTERUUPT INIT DONE]\n");
    value = request_irq(KEYS_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED,
    "pushbutton_irq_hndlr", (void*)(irq_handler));

    return value;
}

static void __exit cleanup_pushbutton_handler(void){
    printk(KERN_INFO "[KEY_exit]\n");
    *LEDR_ptr = 0;
    free_irq(KEYS_IRQ, (void*)irq_handler);
    unregister_chrdev(KEY_major, KEY_NAME);
}


module_init(initialize_pushbutton_handler);
module_exit(cleanup_pushbutton_handler);