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
    uint8_t memory[4096];
    uint8_t V[16];
    uint8_t DT;
    uint8_t ST;
    uint8_t SP;
    uint16_t stack[16];
    uint16_t I;
    uint16_t PC;
} cpu;

void clear_term() {
    printf("\e[H\e[2J");
    printf("\e[3J"); 
}

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
    switch(inst.nnn) {
        case 0x0E0:
            clear_term();
            break;
        case 0x0EE:
            if(handle_stack_over_or_under_flow(cpu)) return ERROR_CODE;
            cpu->PC = cpu->stack[cpu->SP];
            cpu->SP--;
            break;
        default:
            printf("SYS Instruction\n");
            //SYS nnn
            break;
    }

    return CONTINUE_CYCLE;
}

int handle_H4B8(cpu *cpu, instruction inst) {
    switch(inst.n) {
        case 0x0:
            //LD Vx, Vy
            break;
        case 0x1:
            //OR Vx, Vy
            break;
        case 0x2:
            //AND Vx, Vy
            break;
        case 0x3:
            //XOR Vx, Vy
            break;
        case 0x4:
            //ADD Vx, Vy
            break;
        case 0x5:
            //SUB Vx, Vy
            break;
        case 0x6:
            //SHR Vx
            break;
        case 0x7:
            //SUBN Vx, Vy
            break;
        case 0xE:
            //SHL Vx
            break;
        default:
            break;
    }

    return CONTINUE_CYCLE;
}

int handle_H4BE(cpu *cpu, instruction inst) {
    switch (inst.kk) {
        case 0x9E:
            //SKP Vx
            break;
        case 0xA1:
            //SKNP Vx
            break;
        default:
            break;
    }

    return CONTINUE_CYCLE;
}

int handle_H4BF(cpu *cpu, instruction inst) {
    switch (inst.kk) {
        case 0x07:
            //LD Vx, DT
            break;
        case 0x0A:
            //LD Vx, K
            break;
        case 0x15:
            //LD DT, Vx
            break;
        case 0x18:
            //LD ST, Vx
            break;
        case 0x1E:
            //ADD I, Vx
            break;
        case 0x29:
            //LD F, Vx
            break;
        case 0x33:
            //LD B, Vx
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

    return CONTINUE_CYCLE;
}

int handle_instruction(cpu *cpu, instruction inst) {
    switch(inst.h4b) {
        case 0x0:
            return handle_H4B0(cpu, inst);
            break;
        case 0x1:
            cpu->PC = inst.nnn;
            break;
        case 0x2:
            if(handle_stack_over_or_under_flow(cpu)) return ERROR_CODE;
            cpu->stack[cpu->SP] = cpu->PC;
            cpu->SP++;
            cpu->PC = inst.nnn;
            break;
        case 0x3:
            //SE Vx, kk
            break;
        case 0x4:
            //SNE Vx, kk
            break;
        case 0x5:
            //SE Vx, Vy
            break;
        case 0x6:
            //LD Vx, kk
            break;
        case 0x7:
            //ADD Vx, kk
            break;
        case 0x8:
            return handle_H4B8(cpu, inst);
            break;
        case 0x9:
            //SNE Vx, Vy
            break;
        case 0xA:
            //LD I, nnn
            break;
        case 0xB:
            //JP V0, nnn
            break;
        case 0xC:
            //RND Vx, kk
            break;
        case 0xD:
            //DRW Vx, Vy, n
            break;
        case 0xE:
            return handle_H4BE(cpu, inst);
            break;
        case 0xF:
            return handle_H4BF(cpu, inst);
            break;
        default:
            break;
    }

    return CONTINUE_CYCLE;
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

int main(void) {
    cpu cpu = {0};
    setup_memory(&cpu);

    int should_quit = 0;
    int quit_reason = 0;

    while(!should_quit) {
        int code = cycle(&cpu);

        if(code != CONTINUE_CYCLE) {
            should_quit = 1;
            quit_reason = code;
        }
    }

    if(quit_reason == ERROR_CODE) {
        printf("%s", last_cycle_error);
    }
}