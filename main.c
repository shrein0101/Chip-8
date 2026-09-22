#include<stdio.h>
#include<SDL2/SDL.h>
#include "chip8.h"
#define WIDTH 1280
#define HEIGHT 800

int main(int argc, char** argv){
    if(argc<2){
        printf("ERROR: Mention the filename in the format %s <file-name>\n",argv[0]);
        return -1;
    }
    printf("The file is : %s\n",argv[1]);

    SDL_Window *window = SDL_CreateWindow("Chip8",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
    int run = 1;
    Chip8 chip8;
    init_chip8(&chip8);
    load_mem(argv[1],&chip8);
    SDL_Event event;
    while(run){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) run = 0;
        }
        emulate(&chip8);
        Render(chip8.display,renderer);
    }
    printf("Hello World\n");
    SDL_Quit();
}