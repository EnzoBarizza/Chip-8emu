#include "cpu.h"

#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <inttypes.h>

char* last_cycle_error;

const uint8_t builtin_sprites[80] = {
    0xF0,0x90,0x90,0x90,0xF0, // 0
    0x20,0x60,0x20,0x20,0x70, // 1
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

void setup_memory(cpu* cpu) {
    memcpy(cpu->memory, builtin_sprites, 80 * sizeof(uint8_t));
    cpu->PC = (PROGRAM_MEMORY_OFFSET);
}

void load_program(cpu* cpu, uint8_t* buffer, size_t buffer_size) {
    memcpy((cpu->memory + PROGRAM_MEMORY_OFFSET), buffer, buffer_size);
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

void dump_memory(cpu* cpu) {
    FILE* fp = fopen("memdump.hex", "wb");

    if(fp == NULL) {
        printf("Error fopen");
        return;
    }

    fwrite(cpu->memory, sizeof(uint8_t), 4096, fp);
    fclose(fp);
}

instruction decode_instruction(uint16_t inst) {
    return (instruction) {
        .A     = inst >> 12,
        .CD    = inst & 0x00FF,
        .D     = inst & 0x000F,
        .BCD   = inst & 0x0FFF,
        .B     = (inst >> 8) & 0x0F,
        .C     = (inst & 0x00FF) >> 4,
        .ABCD  = inst
    };
}

int handle_stack_over_or_under_flow(cpu* cpu) {
    if(cpu->SP <= 0 || cpu->SP > 16) {
        last_cycle_error = malloc(16 * sizeof(char));
        snprintf(last_cycle_error,
                16 * sizeof(char),
                "stack_%s",
                cpu->SP <= 0 ? "underflow" : "overflow");
        return ERROR_CODE;
    }

    return 0;
}

int handle_H4B0(cpu *cpu, instruction inst, bool decompile) {
    switch(inst.BCD) {
        case 0x0E0:
            if (decompile) {
                printf("CLS\n");
                return 0; 
            }
            i_CLS(cpu);
            break;
        case 0x0EE:
            if (decompile) {
                printf("RET\n");
                return 0; 
            }   
            i_RET(cpu);
            break;
        default:
            if (decompile) {
                printf("SYS %X\n", inst.BCD);
                return 0; 
            }  
            //SYS nnn
            break;
    }

    return CONTINUE_CYCLE;
}

int handle_H4B8(cpu *cpu, instruction inst, bool decompile) {
    int ret = CONTINUE_CYCLE;
    switch(inst.D) {
        case 0x0:
            if (decompile) {
                printf("LD V%X, V%X\n", inst.B, inst.C);
                return 0; 
            } 
            ret = i_LD(cpu, &cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x1:
            if (decompile) {
                printf("OR V%X, V%X\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_OR(cpu, &cpu->V[inst.B], cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x2:
            if (decompile) {
                printf("AND V%X, V%X\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_AND(cpu, &cpu->V[inst.B], cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x3:
            if (decompile) {
                printf("XOR V%X, V%X\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_XOR(cpu, &cpu->V[inst.B], cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x4:
            if (decompile) {
                printf("ADD V%X, V%X\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_ADD(cpu, &cpu->V[inst.B], cpu->V[inst.B] , cpu->V[inst.C], true);
            break;
        case 0x5:
            if (decompile) {
                printf("SUB V%X, V%X\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_SUB(cpu, &cpu->V[inst.B], cpu->V[inst.B] , cpu->V[inst.C], 1);
            break;
        case 0x6:
            if (decompile) {
                printf("SHR V%X {, V%X}\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_SHR(cpu, &cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x7:
            if (decompile) {
                printf("SUBN V%X, V%X\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_SUBN(cpu, &cpu->V[inst.B], cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0xE:
            if (decompile) {
                printf("SHL V%X {, V%X}\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_SHL(cpu, &cpu->V[inst.B], cpu->V[inst.C]);
            break;
        default:
            if (decompile) {
                printf("UNKOWN\n");
                return 0; 
            }
            break;
    }

    return ret;
}

int handle_H4BE(cpu *cpu, instruction inst, bool decompile) {
    switch (inst.CD) {
        case 0x9E:
            if (decompile) {
                printf("SKP V%X\n", inst.B);
                return 0; 
            }
            i_SKP(cpu, cpu->V[inst.B]);
            break;
        case 0xA1:
            if (decompile) {
                printf("SKNP V%X\n", inst.B);
                return 0; 
            }
            i_SKNP(cpu, cpu->V[inst.B]);
            break;
        default:
            if (decompile) {
                printf("UNKNOWN\n");
                return 0; 
            }
            break;
    }

    return CONTINUE_CYCLE;
}

int handle_H4BF(cpu *cpu, instruction inst, bool decompile) {
    int ret = CONTINUE_CYCLE;
    switch (inst.CD) {
        case 0x07:
            if (decompile) {
                printf("LD V%X, DT\n", inst.B);
                return 0; 
            }
            ret = i_LD(cpu, &cpu->V[inst.B], cpu->DT);
            break;
        case 0x0A:
            if (decompile) {
                printf("LD V%X, K\n", inst.B);
                return 0; 
            }
            ret = i_LD_Vx_K(cpu);
            break;
        case 0x15:
            if (decompile) {
                printf("LD DT, V%X\n", inst.B);
                return 0; 
            }
            ret = i_LD(cpu, &cpu->DT, cpu->V[inst.B]);
            break;
        case 0x18:
            if (decompile) {
                printf("LD ST, V%X\n", inst.B);
                return 0; 
            }
            ret = i_LD(cpu, &cpu->ST, cpu->V[inst.B]);
            break;
        case 0x1E:
            if (decompile) {
                printf("LD V%X, V%X\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_ADD16(cpu, &cpu->I, cpu->V[inst.B], cpu->I, 0);
            break;
        case 0x29:
            if (decompile) {
                printf("LD F, V%X\n", inst.B);
                return 0; 
            }
            ret = i_Fx29(cpu, cpu->V[inst.B]);
            break;
        case 0x33:
            if (decompile) {
                printf("LD B, V%X\n", inst.B);
                return 0; 
            }
            ret = i_Fx33(cpu, cpu->V[inst.B]);
            break;
        case 0x55:
            if (decompile) {
                printf("LD [I], V%X\n", inst.B);
                return 0; 
            }
            ret = i_Fx55(cpu, inst.B);
            break;
        case 0x65:
            if (decompile) {
                printf("LD V%X, [I]\n", inst.B);
                return 0; 
            }
            ret = i_Fx65(cpu, inst.B);
            break;
        default:
            break;
    }

    return ret;
}

int handle_instruction(cpu *cpu, instruction inst, bool decompile) {
    uint8_t ret = CONTINUE_CYCLE;
    switch(inst.A) {
        case 0x0:
            ret = handle_H4B0(cpu, inst, decompile);
            break;
        case 0x1:
            if (decompile) {
                printf("JP %X\n", inst.BCD);
                return 0; 
            }   
            ret = i_JP(cpu, inst.BCD);
            break;
        case 0x2:
            if (decompile) {
                printf("CALL %X\n", inst.BCD);
                return 0; 
            }   
            ret = i_CALL(cpu, inst.BCD);
            break;
        case 0x3:
            if (decompile) {
                printf("SE V%X, %X\n", inst.B, inst.CD);
                return 0; 
            }   
            ret = i_SE(cpu, cpu->V[inst.B], inst.CD);
            break;
        case 0x4:
            if (decompile) {
                printf("SNE V%X, %X\n", inst.B, inst.CD);
                return 0; 
            }   
            ret = i_SNE(cpu, cpu->V[inst.B], inst.CD);
            break;
        case 0x5:
            if (decompile) {
                printf("SE V%X, V%X\n", inst.B, inst.C);
                return 0; 
            }   
            ret = i_SE(cpu, cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x6:
            if (decompile) {
                printf("SE V%X, V%X\n", inst.B, inst.C);
                return 0; 
            } 
            ret = i_LD(cpu, &cpu->V[inst.B], inst.CD);
            break;
        case 0x7:
            if (decompile) {
                printf("ADD V%X, %X\n", inst.B, inst.CD);
                return 0; 
            } 
            ret = i_ADD(cpu, &cpu->V[inst.B], cpu->V[inst.B], inst.CD, 0);
            break;
        case 0x8:
            ret = handle_H4B8(cpu, inst, decompile);
            break;
        case 0x9:
            if (decompile) {
                printf("SNE V%X, V%X\n", inst.B, inst.C);
                return 0; 
            }
            ret = i_SNE(cpu, cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0xA:
            if (decompile) {
                printf("LD I, %X\n", inst.BCD);
                return 0; 
            }
            ret = i_LD16(cpu, &cpu->I, inst.BCD);
            break;
        case 0xB:
            if (decompile) {
                printf("JP V0, %X\n", inst.BCD);
                return 0; 
            }
            ret = i_JP(cpu, cpu->V[0x0] + inst.BCD);
            break;
        case 0xC:
            if (decompile) {
                printf("RND V%X, %X\n", inst.C, inst.CD);
                return 0; 
            }
            ret = i_RND(cpu, &cpu->V[inst.B], inst.CD);
            break;
        case 0xD:
            if (decompile) {
                printf("DRW V%X, V%X, %X\n", inst.B, inst.C, inst.D);
                return 0; 
            }
            ret = i_DRW(cpu, inst);
            break;
        case 0xE:
            ret = handle_H4BE(cpu, inst, decompile);
            break;
        case 0xF:
            ret = handle_H4BF(cpu, inst, decompile);
            break;
        default:
            if (decompile) {
                printf("UNKNOWN\n");
                return 0; 
            }
            break;
    }

    return ret;
}

int i_CLS(struct cpu* cpu) {
    #ifdef DEBUG8
    printf("CLS\n");
    #endif
    memset(cpu->screen_buffer, 0, sizeof(uint8_t) * 32 * 64);
    return CONTINUE_CYCLE;
}

int i_RET(cpu* cpu) {
    #ifdef DEBUG8
    printf("RET\n");
    #endif
    if(handle_stack_over_or_under_flow(cpu) == ERROR_CODE) return ERROR_CODE;
    cpu->PC = cpu->stack[--cpu->SP];
    return CONTINUE_CYCLE;
}

int i_JP(cpu* cpu, uint16_t address) {
    #ifdef DEBUG8
    //printf("JP\n");
    #endif
    cpu->PC = address;
    cpu->should_not_increment_pc = 1;
    return CONTINUE_CYCLE;
}

int i_CALL(cpu* cpu, uint16_t address) {
    #ifdef DEBUG8
    printf("CALL\n");
    #endif
    if(cpu->SP > 16) return handle_stack_over_or_under_flow(cpu);
    cpu->stack[cpu->SP++] = cpu->PC;
    cpu->PC = address;
    cpu->should_not_increment_pc = true;

    return CONTINUE_CYCLE;
}

int i_SE(cpu* cpu, uint8_t v1, uint8_t v2) {
    #ifdef DEBUG8
    printf("SE\n");
    #endif
    if(v1 == v2) cpu->PC += 2;
    return CONTINUE_CYCLE;
}

int i_SNE(cpu* cpu, uint8_t v1, uint8_t v2) {
    #ifdef DEBUG8
    printf("SNE\n");
    #endif
    if(v1 != v2) cpu->PC += 2;
    return CONTINUE_CYCLE;
}

int i_LD(cpu* cpu, uint8_t* dest, uint8_t src) {
    #ifdef DEBUG8
    printf("LD\n");
    #endif
    *dest = src;
    return CONTINUE_CYCLE;
}

int i_LD16(cpu* cpu, uint16_t* dest, uint16_t src) {
    #ifdef DEBUG8
    printf("LD16\n");
    #endif
    *dest = src;
    return CONTINUE_CYCLE;
}

int i_ADD(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2, int set_carry) {
    #ifdef DEBUG8
    printf("ADD\n");
    #endif
    if(set_carry) {
        uint16_t add = v1 + v2;
        *dest = (uint8_t)(add & 0xFF);
        cpu->V[0xF] = add > 255;
    } else {
        *dest = v1 + v2;
    }

    return CONTINUE_CYCLE;
}

int i_ADD16(cpu* cpu, uint16_t* dest, uint16_t v1, uint16_t v2, int set_carry) {
    #ifdef DEBUG8
    printf("ADD16\n");
    #endif
    if(set_carry) {
        uint16_t add = v1 + v2;
        cpu->V[0xF] = add > 255;
        *dest = (uint8_t)(add & 0xFF);
    } else {
        *dest = v1 + v2;
    }

    return CONTINUE_CYCLE;
}

int i_OR(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2) {
    #ifdef DEBUG8
    printf("OR\n");
    #endif
    *dest = v1 | v2;
    return CONTINUE_CYCLE;
}

int i_AND(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2) {
    #ifdef DEBUG8
    printf("AND\n");
    #endif
    *dest = v1 & v2;
    return CONTINUE_CYCLE;
}

int i_XOR(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2) {
    #ifdef DEBUG8
    printf("XOR\n");
    #endif
    *dest = v1 ^ v2;
    return CONTINUE_CYCLE;
}

int i_SUB(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2, int set_not_borrow) {
    #ifdef DEBUG8
    printf("SUB\n");
    #endif
    // cpu->V[0xF] = (v1 > v2) ? 1 : 0;
    // *dest = v1 - v2;
    uint8_t sub = v1 - v2;
    uint8_t carry = v1 >= v2;
    *dest = sub & 0xFF;
    cpu->V[0xF] = carry;
    return CONTINUE_CYCLE;
}

int i_SHR(struct cpu* cpu, uint8_t* dest, uint8_t val) {
    #ifdef DEBUG8
    printf("SHR\n");
    #endif
    *dest = val;
    uint8_t carry = (*dest & 0x01);
    *dest >>= 1;
    cpu->V[0xF] = carry;
    return CONTINUE_CYCLE;
}

int i_SHL(struct cpu* cpu, uint8_t* dest, uint8_t val) {
    #ifdef DEBUG8
    printf("SHL\n");
    #endif
    *dest = val;
    uint8_t carry = (*dest & 0x80) != 0;
    *dest <<= 1;
    cpu->V[0xF] = carry;

    return CONTINUE_CYCLE;
}

int i_SUBN(cpu* cpu, uint8_t* dest, uint8_t v1, uint8_t v2) {
    #ifdef DEBUG8
    printf("SUBN\n");
    #endif
    *dest = v2 - v1;
    cpu->V[0xF] = v2 >= v1;
    return CONTINUE_CYCLE;
}

int i_RND(cpu* cpu, uint8_t* dest, uint8_t and) {
    #ifdef DEBUG8
    printf("RND\n");
    #endif
    uint8_t gen = (uint8_t) (((float)rand() / (float)RAND_MAX) * 255);
    return i_AND(cpu, dest, gen, and);
}

int i_DRW(cpu* cpu, instruction inst) {
    #ifdef DEBUG8
    printf("DRW\n");
    #endif
    cpu->V[0xF] = 0;

    uint8_t sprite[inst.D];
    memcpy(&sprite, &(cpu->memory[cpu->I]), inst.D);
    
    uint8_t pos_x = cpu->V[inst.B] % 64;
    uint8_t pos_y = cpu->V[inst.C] % 32;

    for(uint8_t row = 0; row < inst.D; row++) {
        uint8_t sprite_byte = sprite[row];
        for(uint8_t col = 0; col < 8; col++) {
            uint8_t* screen_pixel = &cpu->screen_buffer[pos_y + row][pos_x + col];
            uint8_t sprite_pixel = sprite_byte & (0x80u >> col);

            if(screen_pixel != 0) {
                cpu->V[0xF] = 1;
            }

            *screen_pixel ^= sprite_pixel != 0 ? 0xFF : 0;
        }
    }

    return CONTINUE_CYCLE;
}

int i_SKP(cpu *cpu, uint8_t key) {
    #ifdef DEBUG8
    printf("SKP\n");
    #endif
    if(cpu->keys_pressed[key] == true) {
        cpu->PC += 2;
    }
    return CONTINUE_CYCLE;
}

int i_SKNP(cpu *cpu, uint8_t key) {
    #ifdef DEBUG8
    printf("SKNP\n");
    #endif
    if(cpu->keys_pressed[key] == false) {
        cpu->PC += 2;
    }
    return CONTINUE_CYCLE;
}

int i_LD_Vx_K(cpu *cpu) {
    #ifdef DEBUG8
    printf("LD Vx K\n");
    #endif
    cpu->halted = true;
    return CONTINUE_CYCLE;
}

int i_Fx33(cpu* cpu, uint8_t val) {
    #ifdef DEBUG8
    printf("Fx33\n");
    #endif

    cpu->memory[cpu->I] = (val / 100) % 10;
    cpu->memory[cpu->I + 1] = (val / 10) % 10;
    cpu->memory[cpu->I + 2] = val % 10;
    return CONTINUE_CYCLE;
}

int i_Fx29(cpu* cpu, uint8_t val) {
    #ifdef DEBUG8
    printf("Fx29\n");
    #endif
    cpu->I = val * 5;
    return CONTINUE_CYCLE;
}

int i_Fx55(cpu* cpu, uint8_t x) {
    #ifdef DEBUG8
    printf("Fx55\n");
    #endif
    for(int i = 0; i <= x; i++) {
        cpu->memory[cpu->I + i] = cpu->V[i];
    }
    return CONTINUE_CYCLE;
}

int i_Fx65(cpu* cpu, uint8_t x) {
    #ifdef DEBUG8
    printf("Fx65\n");
    #endif
    for(int i = 0; i <= x; i++) {
        cpu->V[i] = cpu->memory[cpu->I + i];
    }
    return CONTINUE_CYCLE;
}

int cycle(cpu* cpu) {
    double clocked = ((double) (clock() - cpu->last_cycle)) / CLOCKS_PER_SEC;
    double clocked_dtst = ((double) (clock() - cpu->last_dtst_cycle)) / CLOCKS_PER_SEC;

    if(clocked_dtst >= (double)1/60) {
        if(cpu->DT != 0) cpu->DT -= 1;
        if(cpu->ST != 0) cpu->ST -=1;

        cpu->last_dtst_cycle = clock();
    }

    if(clocked >= (double)1/1500) {
        int code = CONTINUE_CYCLE;

        cpu->last_cycle = clock();

        if(!cpu->halted) {
            if(cpu->PC >= sizeof(cpu->memory)) return EOP;
            uint8_t hb = cpu->memory[cpu->PC];
            uint8_t lb = cpu->memory[cpu->PC + 1];

            uint16_t binst = ((uint16_t) hb << 8) | lb;
            instruction inst = decode_instruction(binst);

            code = handle_instruction(cpu, inst, false);

            if(cpu->should_not_increment_pc) cpu->should_not_increment_pc = 0; 
            else cpu->PC += 2;
        }

        return code;
    }
    return CONTINUE_CYCLE;
}

void decompile_next_instruction(cpu* cpu) {
    uint8_t hb = cpu->memory[cpu->PC];
    uint8_t lb = cpu->memory[cpu->PC + 1];

    uint16_t binst = ((uint16_t) hb << 8) | lb;
    instruction inst = decode_instruction(binst);

    handle_instruction(cpu, inst, true);
    cpu->PC += 2;
}
