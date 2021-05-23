#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include "address_map_arm.h"
#include "interrupt_ID.h"

void* LW_virtual;
volatile int* LEDR_ptr;
volatile int* KEY_ptr;
irq_handler_t irq_handler(int irq, void *dev_id, struct pt_regs *regs){
    *LEDR_ptr += 1;
    *(KEY_ptr+3) = 0x1; //엣지캡쳐 제거
    
    return (irq_handler_t) IRQ_HANDLED;
}

static int __init initialize_pushbutton_handler(void){
    printk(KERN_INFO "ledr module init...\n");
    int value;
    LW_virtual = ioremap_nocache(LW_BRIDGE_BASE, LW_BRIDGE_SPAN); //주소값 받아오기

    LEDR_ptr = LW_virtual + LEDR_BASE;
    *LEDR_ptr = 0;

    KEY_ptr = LW_virtual + KEY_BASE;
    *(KEY_ptr+3) = 1; //엣지캡쳐 제거
    *(KEY_ptr+2) = 1; //IRQ 셋팅

    value = request_irq(KEYS_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED,
    "pushbutton_irq_hndlr", (void*)(irq_handler));
    return value;
}

static void __exit cleanup_pushbutton_handler(void){
    printk(KERN_INFO "ledr module clean up...\n");
    *LEDR_ptr = 0;
    free_irq(KEYS_IRQ, (void*)irq_handler);

}

module_init(initialize_pushbutton_handler);
module_exit(cleanup_pushbutton_handler);