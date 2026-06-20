#ifndef _CPU_H
#define _CPU_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define PROGRAM_MEMORY_OFFSET 0x1FF

#define CONTINUE_CYCLE 0
#define EOP 1
#define ERROR_CODE -1

/*
Big Endian.
Let a 0xABCD instruction
*/
typedef struct {
    uint8_t A;      //Highest 4-bits
    uint8_t B;      //Lowest 4-bits of the highest byte
    uint8_t C;      //Upper 4-bits of the lowest byte
    uint8_t D;      //Lowest 4-bits
    uint8_t CD;     //Lowest 8 bits;
    uint16_t BCD;   //Lowest 12-bits
    uint16_t ABCD;  //Full instruction
} instruction;

typedef struct {
    uint8_t memory[4096];
    uint8_t V[16];
    uint8_t DT;
    uint8_t ST;
    uint8_t SP;
    uint8_t should_not_increment_pc;
    uint16_t stack[16];
    uint16_t I;
    uint16_t PC;
} cpu;

void setup_memory(cpu* cpu);
void load_program(cpu* cpu, uint8_t* buffer, size_t buffer_size);
void registry_print(const char* name, uint16_t reg);
void dump_registers(cpu* cpu);
void dump_memory(cpu* cpu);
instruction decode_instruction(uint16_t inst);
int handle_stack_over_or_under_flow(cpu* cpu);
int handle_H4B0(cpu *cpu, instruction inst);
int handle_H4B8(cpu *cpu, instruction inst);
int handle_H4BE(cpu *cpu, instruction inst);
int handle_H4BF(cpu *cpu, instruction inst);
int handle_instruction(cpu *cpu, instruction inst);
int cycle(cpu* cpu);

#endif
