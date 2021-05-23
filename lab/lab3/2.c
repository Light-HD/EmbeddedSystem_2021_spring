#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "address_map_arm.h"

    // 숫자 미리 정의
int hexNum[] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 
                0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111};

void print_arg(volatile int *hex30, int x){
    if(x==1) *hex30 = 0b11101110101000001101111; //print Arg
    else if(x==2) *hex30 = 0b0011111101110011; //print OP
}

int input_arg(volatile int *key, volatile int *hex30){
    int arg=0;

    //키 누르는 내내 시행
    while(*key&0x1){
        if(*key&0x1) arg = (arg+1) % 10;
        *hex30 = hexNum[arg];
        usleep(300000); //0.3초 delay
    }
    return arg;
}

int input_op(volatile int *key, volatile int *hex30){
    int op=0;
    while(*key&0x1){ //op 입력모드 - 0 1 2 3 순서대로 + - x /
        *hex30 = hexNum[op];
        usleep(300000); //0.3초 delay
        if(*key&0x1) op = (op+1) % 4;
    }
}


int main(){
    int fd, arg1=0, arg2=0, op=0, result, minus_sign, err, cal_more;
    void *lw_virtual;
    volatile int *hex30, *hex54, *key;

    fd = open("/dev/mem", (O_RDWR | O_SYNC));
    lw_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, LW_BRIDGE_BASE);
    hex30 = (volatile int *)(lw_virtual + HEX3_HEX0_BASE);
    hex54 = (volatile int *)(lw_virtual + HEX5_HEX4_BASE);
    key = (volatile int *)(lw_virtual + KEY_BASE);




    *hex30 = 0; *hex54 = 0;
    while(1){
        arg1=0; arg2=0; op=0; minus_sign=0; err=0; cal_more=0;

        while(! (*key)) print_arg(hex30, 1); //key가 눌리기 전까지는 'Arg' 출력하며 대기
        arg1 = input_arg(key, hex30);
        usleep(2000000); //2초동안 arg1에 뭘 넣었는지 display

        //두번째~n번째 인자 입력받기
        do{
            while(! (*key)) print_arg(hex30, 3); //key가 눌리기 전까지는 'OP' 출력하며 대기
            op = input_op(key, hex30);
            usleep(2000000); //2초동안 OP에 뭘 넣었는지 display

            while(! (*key)) print_arg(hex30, 2); //key가 눌리기 전까지는 'Arg' 출력하며 대기
            arg2 = input_arg(key, hex30);

            //계산을 더 할지? 안할지?
            while(! (*key)); //일단 누를 때까지 대기
            usleep(300000); //0.3초안에 재클릭하면
            

        }while(cal_more)

        if(op == 0) result = arg1 + arg2;
        else if(op == 1) result = arg1 - arg2;
        else if(op == 2) result = arg1 * arg2;
        else{
            if(arg2 == 0) err = 1; //0으로 나눗셈 방지
            else result = arg1 / arg2;
        }

        if(err) *hex30 = 0b11110010101000001010000; //0으로 나눠지면 Err 표기
        else{ //아니면 결과값 표기
            if(result < 0){ //음수일때는 부호표시 필수
                result = ~result + 1;
                minus_sign = 1;
            }
            *hex30 = (hexNum[result/10] <<8) | (hexNum[result%10]);
            if(minus_sign) *hex30 = *hex30 | (0b01000000<<16);
        }
        usleep(5000000); //5초동안 결과출력
    }

    munmap(lw_virtual, LW_BRIDGE_BASE);
    close(fd);
    return 0;
}