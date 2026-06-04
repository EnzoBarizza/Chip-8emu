#include "instructions.h"
#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int i_display_device_CLS(cpu *cpu) {
    printf("\e[H\e[2J");
    printf("\e[3J"); 
    return CONTINUE_CYCLE;
}

int i_RET(cpu* cpu) {
    if(handle_stack_over_or_under_flow(cpu)) return ERROR_CODE;
    cpu->PC = cpu->stack[cpu->SP];
    cpu->SP--;
    return CONTINUE_CYCLE;
}

int i_JP(cpu* cpu, uint16_t address) {
    cpu->PC = address;
    return CONTINUE_CYCLE;
}

int i_CALL(cpu* cpu, uint16_t address) {
    if(handle_stack_over_or_under_flow(cpu)) return ERROR_CODE;
    cpu->stack[cpu->SP] = cpu->PC;
    cpu->SP++;
    cpu->PC = address;

    return CONTINUE_CYCLE;
}

int i_SE(cpu* cpu, uint8_t v1, uint8_t v2) {
    if(v1 == v2) cpu->PC += 2;
    return CONTINUE_CYCLE;
}

int i_SNE(cpu* cpu, uint8_t v1, uint8_t v2) {
    if(v1 != v2) cpu->PC += 2;
    return CONTINUE_CYCLE;
}

int i_LD(cpu* cpu, uint8_t* dest, uint8_t src) {
    *dest = src;
    return CONTINUE_CYCLE;
}

int i_LD16(cpu* cpu, uint16_t* dest, uint16_t src) {
    *dest = src;
    return CONTINUE_CYCLE;
}

int i_ADD(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2, int set_carry) {
    if(set_carry) {
        uint16_t add = v1 + v2;
        cpu->V[0xF] = add > 255;
        *dest = (uint8_t)(add & 0x00FF);
    } else {
        *dest = v1 + v2;
    }
    
    return CONTINUE_CYCLE;
}

int i_ADD16(cpu* cpu, uint16_t* dest, uint16_t v1, uint16_t v2, int set_carry) {
    if(set_carry) {
        uint16_t add = v1 + v2;
        cpu->V[0xF] = add > 255;
        *dest = (uint8_t)(add & 0x00FF);
    } else {
        *dest = v1 + v2;
    }
    
    return CONTINUE_CYCLE;
}

int i_OR(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2) {
    *dest = v1 | v2;
    return CONTINUE_CYCLE;
}

int i_AND(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2) {
    *dest = v1 & v2;
    return CONTINUE_CYCLE;
}

int i_XOR(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2) {
    *dest = v1 ^ v2;
    return CONTINUE_CYCLE;
}

int i_SUB(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2, int set_not_borrow) {
    cpu->V[0xF] = v1 > v2;
    *dest = v1 - v2;
    return CONTINUE_CYCLE;
}

int i_SHR(cpu* cpu, uint8_t* victim) {
    cpu->V[0xF] = *victim & (0b00000001);
    *victim >>= 1;
    return CONTINUE_CYCLE;
}

int i_SUBN(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2) {
    cpu->V[0xF] = v2 > v1;
    *dest = v2 - v1;
    return CONTINUE_CYCLE;
}

int i_SHL(cpu* cpu, uint8_t* victim) {
    cpu->V[0xF] = *victim >> 7;
    *victim <<= 1;
    return CONTINUE_CYCLE;
}

int i_RND(cpu* cpu, uint8_t* dest, uint8_t and) {
    uint8_t gen = (uint8_t) (((float)rand() / (float)RAND_MAX) * 255);
    return i_AND(cpu, dest, gen, and);
}

int i_display_device_DRW(cpu* cpu) {
    printf("DRW WIP");
    return CONTINUE_CYCLE;
}

int i_input_device_SKP(cpu *cpu) {
    printf("SKP WIP");
    return CONTINUE_CYCLE;
}

int i_input_device_SKNP(cpu *cpu) {
    printf("SKNP WIP");
    return CONTINUE_CYCLE;
}

int i_LD_Fx33(cpu* cpu, uint8_t val) {
    cpu->memory[cpu->I + 2] = val % 10;
    cpu->memory[cpu->I + 1] = (val / 10) % 10;
    cpu->memory[cpu->I]     = val / 100;

    return CONTINUE_CYCLE;
}