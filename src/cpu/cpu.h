#ifndef _CPU_H
#define _CPU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#define PROGRAM_MEMORY_OFFSET 0x200

#define CONTINUE_CYCLE 0
#define EOP 1
#define ERROR_CODE 2

typedef struct cpu {
    uint8_t memory[4096];
    uint8_t V[16];
    uint8_t DT;
    uint8_t ST;
    uint8_t SP;
    uint8_t should_not_increment_pc;
    uint16_t stack[16];
    uint16_t I;
    uint16_t PC;
    uint8_t screen_buffer[32][64];
    clock_t last_cycle;
    clock_t last_dtst_cycle;
} cpu;

/*
Big Endian.
Let a 0xABCD instruction
*/
typedef struct instruction {
    uint8_t A;      //Highest 4yxy-bits
    uint8_t B;      //Lowest 4-bits of the highest byte
    uint8_t C;      //Upper 4-bits of the lowest byte
    uint8_t D;      //Lowest 4-bits
    uint8_t CD;     //Lowest 8 bits;
    uint16_t BCD;   //Lowest 12-bits
    uint16_t ABCD;  //Full instruction
} instruction;

void setup_memory(cpu* cpu);
void load_program(cpu* cpu, uint8_t* buffer, size_t buffer_size);
void registry_print(const char* name, uint16_t reg);
void dump_registers(cpu* cpu);
void dump_memory(cpu* cpu);
struct instruction decode_instruction(uint16_t inst);
int handle_stack_over_or_under_flow(cpu* cpu);
int handle_H4B0(cpu *cpu, struct instruction inst, bool decompile);
int handle_H4B8(cpu *cpu, struct instruction inst, bool decompile);
int handle_H4BE(cpu *cpu, struct instruction inst, bool decompile);
int handle_H4BF(cpu *cpu, struct instruction inst, bool decompile);
int handle_instruction(cpu *cpu, struct instruction inst, bool decompile);
int cycle(cpu* cpu);
void decompile_next_instruction(cpu* cpu);

//Common Instructions
int i_RET(struct cpu* cpu);
int i_JP(struct cpu* cpu, uint16_t address);
int i_CALL(struct cpu* cpu, uint16_t address);
int i_SE(struct cpu* cpu, uint8_t v1, uint8_t v2);
int i_SNE(struct cpu* cpu, uint8_t v1, uint8_t v2);
int i_LD(struct cpu* cpu, uint8_t* dest, uint8_t src);
int i_LD16(struct cpu* cpu, uint16_t* dest, uint16_t src);
int i_ADD(struct cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2, int set_carry);
int i_ADD16(struct cpu* cpu, uint16_t* dest, uint16_t v1, uint16_t v2, int set_carry);
int i_OR(struct cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2);
int i_AND(struct cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2);
int i_XOR(struct cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2);
int i_SUB(struct cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2, int set_not_borrow);
int i_SHR(struct cpu* cpu, uint8_t* victim);
int i_SUBN(struct cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2);
int i_SHL(struct cpu* cpu, uint8_t* victim);
int i_RND(struct cpu* cpu, uint8_t* dest, uint8_t and);

//Device Instructions
int i_display_device_CLS(struct cpu* cpu);
int i_display_device_DRW(struct cpu* cpu, instruction inst);
int i_input_device_SKP(struct cpu* cpu);
int i_input_device_SKNP(struct cpu* cpu);
int i_input_device_LD_Vx_K(struct cpu* cpu);

//Unique Instructions
int i_Fx33(struct cpu* cpu, uint8_t val);
int i_Fx29(struct cpu* cpu, uint8_t val);
int i_Fx55(struct cpu* cpu, uint8_t x);
int i_Fx65(struct cpu* cpu, uint8_t x);

#endif
