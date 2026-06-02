#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define PROGRAM_MEMORY_OFFSET 0x1FF

typedef uint8_t byte;
typedef uint8_t register8;
typedef uint16_t register16;

const byte builtin_sprites[80] = {
    0xF0,0x90,0x90,0x90,0xF0,      // 0
    0x20,0x60,0x20,0x20,0x70,      // 1
    0xF0,0x10,0xF0,0x80,0xF0, // 2
    0xF0,0x10,0xF0,0x10,0xF0, // 3
    0x90,0x90,0xF0,0x10,0x10, // 4
    0xF0,0x80,0xF0,0x10,0xF0, // 5
    0xF0,0x80,0xF0,0x90,0xF0, // 6
    0xF0,0x10,0x20,0x40,0x40, // 7
    0xF0,0x90,0xF0,0x90,0xF0, // 8
    0xF0,0x90,0xF0,0x10,0xF0, // 9
    0xF0,0x90,0xF0,0x90,0x90, // A
    0xE0,0x90,0xE0,0x90,0xE0, // B
    0xF0,0x80,0x80,0x80,0xF0, // C
    0xE0,0x90,0x90,0x90,0xE0, // D
    0xF0,0x80,0xF0,0x80,0xF0, // E
    0xF0,0x80,0xF0,0x80,0x80  // F
};

//Big endian
typedef struct {
    uint8_t h4b;    //Highest 4-bits
    uint8_t n;      //Lowest 4-bits
    uint8_t x;      //Lowest 4-bits of the highest byte
    uint8_t y;      //Upper 4-bits of the lowest byte
    uint8_t kk;     //Lowest 8 bits;
    uint16_t nnn;   //Lowest 12-bits
    uint16_t finst; //Full instruction
} instruction;

typedef struct {
    byte memory[4096];
    register8 V[16];
    register8 DT;
    register8 ST;
    register8 SP;
    register16 stack[16];
    register16 I;
    register16 PC;
} cpu;

void clear_term() {
    printf("\e[H\e[2J");
    printf("\e[3J"); 
}

void setup_memory(cpu* cpu) {
    memcpy(cpu->memory, builtin_sprites, 80 * sizeof(byte));
    cpu->PC = (PROGRAM_MEMORY_OFFSET + 1);
}

void load_program(cpu* cpu, byte* buffer, size_t buffer_size) {
    memcpy((cpu->memory + PROGRAM_MEMORY_OFFSET + 1), buffer, buffer_size);
}

void registry_print(const char* name, uint16_t reg) {
    printf("%s = 0x%x = %i\n", name, reg, reg);
}

void dump_registers(cpu* cpu) {
    for(int i = 0; i < 16; i++) {
        char name[4];
        sprintf(name, "V%X", i);
        registry_print(name, cpu->V[i]);
    }
    registry_print("DT", cpu->DT);
    registry_print("ST", cpu->ST);
    registry_print("SP", cpu->SP);
    registry_print("IR", cpu->I);
    registry_print("PC", cpu->PC);
}

void dump_memory(cpu cpu) {
    FILE* fp = fopen("memdump.hex", "wb");

    if(fp == NULL) {
        printf("Error fopen");
        return;
    }

    fwrite(cpu.memory, sizeof(byte), 4096, fp);
}

instruction decode_instruction(uint16_t inst) {
    return (instruction) {
        .h4b   = inst >> 12,
        .kk    = inst & 0x00FF,
        .n     = inst & 0x000F,
        .nnn   = inst & 0x0FFF,
        .x     = (inst >> 8) & 0x0F,
        .y     = (inst & 0x00FF) >> 4,
        .finst = inst
    };
}

void print_instruction(instruction inst) {
    printf("h4b: %x\n", inst.h4b);
    printf("kk: %x\n", inst.kk);
    printf("n: %x\n", inst.n);
    printf("nnn: %x\n", inst.nnn);
    printf("x: %x\n", inst.x);
    printf("y: %x\n", inst.y);
}



int main(void) {
    cpu cpu = {0};
    setup_memory(&cpu);
    
    instruction inst = decode_instruction(0xF265);
}