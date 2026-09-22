#include<stdio.h>
#include<unistd.h>
#include<termios.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<SDL2/SDL.h>
#include"chip8.h"
#define FONT_START 0x50

unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A 
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void init_chip8(Chip8* chip){
    *chip = (Chip8){0};
    memcpy(chip->memory + FONT_START,fontset,sizeof(fontset));
    chip->pc = 0x200;
    srand(time(NULL));
}

void load_mem(const char *file, Chip8 *chip){
    FILE *f = fopen(file,"rb");
    if(f==NULL){
        printf("ERROR: The file could not be open\n");
        return;
    }
    fseek(f,0,SEEK_END);
    unsigned long size = ftell(f);
    chip->rom_end = 0x200 + size;
    fseek(f,0,SEEK_SET);
    if(size > sizeof(chip->memory) - 0x200){
        printf("The ROM is too big\n");
        fclose(f);
        return;
    }
    fread(chip->memory+0x200,1,size,f);
    fclose(f);
}

uint8_t random_byte(){
  return rand()%256;
}

uint8_t keypress(char in){
  switch(in){

    case '1' : return 0x1;
    case '2' : return 0x2;
    case '3' : return 0x3;
    case '4' : return 0xC;
    case 'q' : return 0x4;
    case 'w' : return 0x5;
    case 'e' : return 0x6;
    case 'r' : return 0xD;
    case 'a' : return 0x7;
    case 's' : return 0x8;
    case 'd' : return 0x9;
    case 'f' : return 0xE;
    case 'z' : return 0xA;
    case 'x' : return 0x0;
    case 'c' : return 0xB;
    case 'v' : return 0xF;
    default : return 0xFF;
  }
}

int emulate(Chip8 *chip){
  
  int running = chip->pc + 1 < chip->rom_end;
  if(!running) return running;

  uint16_t op = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];
  chip->pc += 2;
  uint8_t instruction = (op & 0xF000) >> 12;
  uint8_t X = (op & 0x0F00) >> 8;
  uint8_t Y = (op & 0x00F0) >> 4;
  uint8_t N = op & 0x000F;
  uint8_t NN = op & 0x00FF;
  uint16_t NNN = op & 0x0FFF;

  switch (instruction){
  case 0x0:
    if (op == 0x00EE){
      chip->sp--;
      chip->pc = chip->stack[chip->sp];
    }
    else if (op == 0x00E0){
      memset(chip->display, 0, sizeof(chip->display));
    }
    break;

  case 0x1:
    chip->pc = NNN;
    break;

  case 0x2:
    chip->stack[chip->sp] = chip->pc;
    chip->sp++;
    chip->pc = NNN;
    break;  

  case 0x3:
    if (chip->V[X] == NN)
      chip->pc += 2;
    break;

  case 0x4:
    if (chip->V[X] != NN)
      chip->pc += 2;
    break;

  case 0x5:
    if (N == 0 && chip->V[X] == chip->V[Y])
      chip->pc += 2;
    break;

  case 0x6:
    chip->V[X] = NN;
    break;

  case 0x7:
    chip->V[X] += NN;
    break;
  
  case 0x8:
    switch (N){
    case 0:
      chip->V[X] = chip->V[Y];
      break;
    case 1:
      chip->V[X] |= chip->V[Y];
      break;
    case 2:
      chip->V[X] &= chip->V[Y];
      break;
    case 3:
      chip->V[X] ^= chip->V[Y];
      break;
    case 4:
      uint16_t sum = chip->V[X] + chip->V[Y];
      chip->V[0xF] = sum > 255;
      chip->V[X] = sum;
      break;
    case 5:
      chip->V[0xF] = chip->V[X] >= chip->V[Y];
      chip->V[X] -= chip->V[Y];
      break;
    case 6:
      chip->V[0xF] = (chip->V[X] & 1);
      chip->V[X] >>= 1;
      break;
    case 7:
      chip->V[0xF] = chip->V[Y] >= chip->V[X];
      chip->V[X] = chip->V[Y] - chip->V[X];
      break;
    case 0xE:
      chip->V[0xF] = (chip->V[X] & 0x80) >> 7;
      chip->V[X] <<= 1;
      break;
    }
    break;
    
  case 0x9:
    if (chip->V[X] != chip->V[Y] && N == 0)
      chip->pc += 2;
    break;

  case 0xA:
    chip->I = NNN;
    break;

  case 0xB:
    chip->pc = NNN + chip->V[0];
    break;

  case 0xC:
    chip->V[X] = random_byte() & NN;
    break;  
  
  case 0xD:
    chip->V[0xF] = 0;
    for (int i = 0; i < N; i++){
      uint8_t sprite = chip->memory[chip->I + i];
      for (int bit = 0; bit < 8; bit++){

        int y = chip->V[Y] + i;
        int x = chip->V[X] + bit;

        if (x > 63)
          x %= 64;
        if (y > 31)
          y %= 32;
        uint8_t temp = chip->display[y][x];
        uint8_t pixel = (sprite >> (7 - bit)) & 1;

        if (pixel && temp)
          chip->V[0xF] = 1;
        chip->display[y][x] ^= pixel;
      }
    }
    /*for (int y = 0; y < 32; y++){
      for (int x = 0; x < 64; x++){
        if (chip->display[y][x] == 0){
          printf(".");
        }
        else{
          printf("&");
        }
      }
      printf("\n");
    }*/
    break;

  case 0xE:
    switch(NN){
      case 0x9E:
        if(chip->keypad[chip->V[X]]) chip->pc += 2;
        break;
      case 0xA1:
        if(!chip->keypad[chip->V[X]]) chip->pc += 2;
        break; 
    }
    break;
    
  case 0xF:
    switch(NN){
      case 0x07:
        chip->V[X] = chip->delay_timer;
        break;
      case 0x15:
        chip->delay_timer = chip->V[X];
        break;
      case 0x18:
        chip->sound_timer = chip->V[X];
        break;
      case 0x1E:
        chip->I += chip->V[X];
        break;
      case 0x33:
        chip->memory[chip->I] = chip->V[X] /100;
        chip->memory[chip->I + 1] = (chip->V[X] /10) % 10;
        chip->memory[chip->I + 2] = chip->V[X] % 10;
        break;
      case 0x55:
        for(int i = 0; i <= X; i++){
          chip->memory[chip->I + i] = chip->V[i]; 
        }
        break;
      case 0x65:
        for(int i = 0; i <= X; i++){
          chip->V[i] = chip->memory[chip->I + i]; 
        }
        break;
      case 0x29:
        chip->I = FONT_START + chip->V[X]*5;
        break;
      case 0x0A:
        char in;
        scanf("%c",&in);
        chip->V[X] = keypress(in);
        break;
      }
      break;
  
  }
  return running;
}
