#include <stdio.h>
#include <stdlib.h>
/*
사용자프로그램과의 인터럽트 혼용 안
1. key를 누르면 숫자가 증가하다가, 내려간다
2. 
*/
int main(int argc, char *argv[]){
    FILE *fp_hex, *fp_key;
    int k = 0;
    fp_hex = fopen("/dev/HEX30", "w");
    //fp_key = fopen("/dev/key", "r");
    while(1){
        fp_key = fopen("/dev/key", "r");//
        if(fgetc(fp_key))
            fputc(abs((k++)%10), fp_hex);
        else
            fputc(abs((k--)%10), fp_hex);
        fflush(fp_hex);
        usleep(500000);
        fclose(fp_key);//
    }

    fclose(fp_hex);
    //fclose(fp_key);
    return 0;
}