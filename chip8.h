#ifndef CHIP8_H
#define CHIP8_H
#include<stdint.h>
typedef struct{
    uint8_t memory[4096];
    uint8_t V[16]; //V0-VF registers
    uint16_t pc; // program counter
    uint16_t I; // index register 

    uint16_t stack[16];
    uint8_t sp;

    uint8_t delay_timer;
    uint8_t sound_timer;
    
    uint8_t display[32][64];

    uint8_t keypad[16]; 
    uint16_t rom_end; // When does the ROM end? 

}Chip8;

extern int running;
void init_chip8(Chip8* chip);
void load_mem(char *file, Chip8 *chip);
int emulate(Chip8 *chip8);
uint8_t random_byte();
#endif