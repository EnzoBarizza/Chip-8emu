#include "chip8.h"
#include "instructions.h"
#include <time.h>

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
    cpu->PC = (PROGRAM_MEMORY_OFFSET + 1);
}

void load_program(cpu* cpu, uint8_t* buffer, size_t buffer_size) {
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

void dump_memory(cpu* cpu) {
    FILE* fp = fopen("memdump.hex", "wb");

    if(fp == NULL) {
        printf("Error fopen");
        return;
    }

    fwrite(cpu->memory, sizeof(uint8_t), 4096, fp);
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

int handle_H4B0(cpu *cpu, instruction inst) {
    switch(inst.BCD) {
        case 0x0E0:
            i_display_device_CLS(cpu);
            break;
        case 0x0EE:
            i_RET(cpu);
            break;
        default:
            //SYS nnn
            break;
    }

    return CONTINUE_CYCLE;
}

int handle_H4B8(cpu *cpu, instruction inst) {
    int ret = CONTINUE_CYCLE;
    switch(inst.D) {
        case 0x0:
            ret = i_LD(cpu, &cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x1:
            ret = i_OR(cpu, &cpu->V[inst.B], cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x2:
            ret = i_AND(cpu, &cpu->V[inst.B], cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x3:
            ret = i_XOR(cpu, &cpu->V[inst.B], cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x4:
            ret = i_ADD(cpu, &cpu->V[inst.B], cpu->V[inst.B] , cpu->V[inst.C], 1);
            break;
        case 0x5:
            ret = i_SUB(cpu, &cpu->V[inst.B], cpu->V[inst.B] , cpu->V[inst.C], 1);
            break;
        case 0x6:
            ret = i_SHR(cpu, &cpu->V[inst.B]);
            break;
        case 0x7:
            ret = i_SUBN(cpu, &cpu->V[inst.B], cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0xE:
            ret = i_SHL(cpu, &cpu->V[inst.B]);
            break;
        default:
            break;
    }

    return ret;
}

int handle_H4BE(cpu *cpu, instruction inst) {
    switch (inst.CD) {
        case 0x9E:
            i_input_device_SKP(cpu);
            break;
        case 0xA1:
            i_input_device_SKNP(cpu);
            break;
        default:
            break;
    }

    return CONTINUE_CYCLE;
}

int handle_H4BF(cpu *cpu, instruction inst) {
    int ret = CONTINUE_CYCLE;
    switch (inst.CD) {
        case 0x07:
            ret = i_LD(cpu, &cpu->V[inst.B], cpu->DT);
            break;
        case 0x0A:
            //LD Vx, K
            break;
        case 0x15:
            ret = i_LD(cpu, &cpu->DT, cpu->V[inst.B]);
            break;
        case 0x18:
            ret = i_LD(cpu, &cpu->ST, cpu->V[inst.B]);
            break;
        case 0x1E:
            ret = i_ADD16(cpu, &cpu->I, cpu->V[inst.B], cpu->I, 0);
            break;
        case 0x29:
            //LD F, Vx
            break;
        case 0x33:
            i_LD_Fx33(cpu, cpu->V[inst.B]);
            break;
        case 0x55:
            //LD [I], Vx
            break;
        case 0x65:
            //LD Vx, [I]
            break;
        default:
            break;
    }

    return ret;
}

int handle_instruction(cpu *cpu, instruction inst) {
    uint8_t ret = CONTINUE_CYCLE;
    switch(inst.A) {
        case 0x0:
            ret = handle_H4B0(cpu, inst);
            break;
        case 0x1:
            ret = i_JP(cpu, inst.BCD);
            break;
        case 0x2:
            ret = i_CALL(cpu, inst.BCD);
            break;
        case 0x3:
            ret = i_SE(cpu, cpu->V[inst.B], inst.CD);
            break;
        case 0x4:
            ret = i_SNE(cpu, cpu->V[inst.B], inst.CD);
            break;
        case 0x5:
            ret = i_SE(cpu, cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0x6:
            ret = i_LD(cpu, &cpu->V[inst.B], inst.CD);
            break;
        case 0x7:
            ret = i_ADD(cpu, &cpu->V[inst.B], cpu->V[inst.B], inst.CD, 0);
            break;
        case 0x8:
            ret = handle_H4B8(cpu, inst);
            break;
        case 0x9:
            ret = i_SNE(cpu, cpu->V[inst.B], cpu->V[inst.C]);
            break;
        case 0xA:
            ret = i_LD16(cpu, &cpu->I, inst.BCD);
            break;
        case 0xB:
            ret = i_JP(cpu, cpu->V[0x0] + inst.BCD);
            break;
        case 0xC:
            ret = i_RND(cpu, &cpu->V[inst.B], inst.CD);
            break;
        case 0xD:
            ret = i_display_device_DRW(cpu);
            break;
        case 0xE:
            ret = handle_H4BE(cpu, inst);
            break;
        case 0xF:
            ret = handle_H4BF(cpu, inst);
            break;
        default:
            break;
    }

    return ret;
}

int cycle(cpu* cpu) {
    if(cpu->PC >= sizeof(cpu->memory)) return EOP;
    uint8_t hb = cpu->memory[cpu->PC];
    uint8_t lb = cpu->memory[cpu->PC + 1];

    uint16_t binst = ((uint16_t) hb << 8) | lb;
    instruction inst = decode_instruction(binst);

    int code = handle_instruction(cpu ,inst);

    cpu->PC += 2;

    return code;
}