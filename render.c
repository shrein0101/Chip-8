#include "chip8.h"
#include<SDL2/SDL.h>
#include<stdint.h>
#include<stdlib.h>

void Render(uint8_t display[][64],SDL_Renderer *renderer){
    
    SDL_Rect pixel;
    pixel.w = 20;
    pixel.h = 25;
    SDL_Surface *surface = SDL_CreateRGBSurface(0,1,1,32,0,0,0,0);
    SDL_FillRect(surface,NULL,SDL_MapRGB(surface->format,255,255,255));
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer,surface);

    for (int i = 0; i < 32; i++){
        for (int j = 0; j < 64; j++){
            pixel.x = j*20;
            pixel.y = i*25;
            if (display[i][j] == 1)
                SDL_RenderCopy(renderer, texture, NULL, &pixel);
        }
    }
    SDL_RenderPresent(renderer);
}