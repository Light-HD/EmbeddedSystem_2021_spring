#include <stdio.h>
#include <stdlib.h>

FILE *fp;


int main(int argc, char *argv[]){
    int ch = 0;
    int cnt = 0;
    

    fp = fopen("/dev/HEX30", "w");

    for(; cnt < 100; cnt++){
        fputc(ch++, fp);
        fflush(fp);
        usleep(50000);
    }

    
    fclose(fp);
    return 0;
}