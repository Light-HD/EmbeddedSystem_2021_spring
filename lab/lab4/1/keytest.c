#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int ret;
    FILE *fp;
    
    while(1){
        fp = fopen("/dev/key", "r");
        printf("ret : %d \n", fgetc(fp));
        usleep(1000000);
        fclose(fp);
    }

    return 0;
}