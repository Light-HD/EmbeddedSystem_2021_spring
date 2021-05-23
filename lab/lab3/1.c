#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "address_map_arm.h"

void shift1(volatile int *hex54, volatile int *hex30, int num){ //1만큼 shift하고, 숫자 삽입
    int over30 = (*hex30 & 0xff000000)>>24;
    *hex54 = (*hex54 << 8 | over30);
    *hex30 = (*hex30 << 8 | num);
}

int main(){
    int fd, k;
    void *lw_virtual;
    volatile int *hex30, *hex54;

    fd = open("/dev/mem", (O_RDWR | O_SYNC));
    lw_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, LW_BRIDGE_BASE);
    hex30 = (volatile int *)(lw_virtual + HEX3_HEX0_BASE);
    hex54 = (volatile int *)(lw_virtual + HEX5_HEX4_BASE);

    // 숫자 미리 정의
    int hexNum[] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 
                            0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111};

    int studentID[] = {2,0,1,6,1,2,4,0,3,6,
                        2,0,1,6,1,2,4,0,8,7,
                        2,0,1,6,1,2,4,2,1,7};

    *hex30 = 0; *hex54 = 0;

    while(1){
        for(k=0;k<30;k++){
            shift1(hex54, hex30, hexNum[studentID[k]]);
            usleep(500000); //0.5초 delay
        }
        for(k=0;k<6;k++){
            shift1(hex54, hex30, 0);
            usleep(500000);
        }
    }

    munmap(lw_virtual, LW_BRIDGE_BASE);
    close(fd);
    return 0;
}