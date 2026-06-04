#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "chip8.h"

//Common Instructions
int i_RET(cpu* cpu);
int i_JP(cpu* cpu, uint16_t address);
int i_CALL(cpu* cpu, uint16_t address);
int i_SE(cpu* cpu, uint8_t v1, uint8_t v2);
int i_SNE(cpu* cpu, uint8_t v1, uint8_t v2);
int i_LD(cpu* cpu, uint8_t* dest, uint8_t src);
int i_LD16(cpu* cpu, uint16_t* dest, uint16_t src);
int i_ADD(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2, int set_carry);
int i_ADD16(cpu* cpu, uint16_t* dest, uint16_t v1, uint16_t v2, int set_carry);
int i_OR(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2);
int i_AND(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2);
int i_XOR(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2);
int i_SUB(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2, int set_not_borrow);
int i_SHR(cpu* cpu, uint8_t* victim);
int i_SUBN(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2);
int i_SHL(cpu* cpu, uint8_t* victim);
int i_RND(cpu* cpu, uint8_t* dest, uint8_t and);


//Unique Instructions
int i_display_device_CLS(cpu* cpu);
int i_display_device_DRW(cpu* cpu);
int i_input_device_SKP(cpu* cpu);
int i_input_device_SKNP(cpu* cpu);
int i_LD_Fx33(cpu* cpu, uint8_t val);
#endif