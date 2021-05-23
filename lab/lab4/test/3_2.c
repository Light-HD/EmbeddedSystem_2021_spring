#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int ch = 0x1;
    int cnt = 0;
    FILE *fp = fopen("/dev/ledr", "w");

    for(; cnt < 10; cnt++){
        fputc(ch, fp);
        fflush(fp);
        ch <<= 1;
        usleep(50000);
    }
    fclose(fp);
    return 0;
}