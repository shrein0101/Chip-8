#include<stdio.h>
#include "chip8.h"

int main(int argc, char** argv){
    if(argc<2){
        printf("ERROR: Mention the filename in the format %s <file-name>\n",*(argv));
        return 0;
    }
    printf("The file is : %s\n",*(argv+1));
    int run = 1;
    Chip8 chip8;
    init_chip8(&chip8);
    load_mem(*(argv+1),&chip8);
    while(run){
        run = emulate(&chip8);
    }
    printf("Hello World\n");
}