#include "runner.h"
#include "cpu.h"
#include <display.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <threads.h>

void run(cpu* cpu) {
    uint8_t code = CONTINUE_CYCLE;
    while(code != EOP) {
        code = cycle(cpu);
        run_frame(cpu->screen_buffer);
        dump_memory(cpu);
    }
}

void decompile(cpu* cpu, size_t size) {
    size_t instruction_count = size / 2;
    for(size_t i = 0; i < instruction_count; i++) {
        decompile_next_instruction(cpu);
    }
}

int main(int argc, const char* argv[]) {
    if(argc < 3) {
        printf("Usage: runner <run|decompile> <path_to_program>\n");
        return -1;
    }

    init_display_device();
    cpu cpu = {0};
    setup_memory(&cpu);

    FILE* file = fopen(argv[2], "rb+");
    if(file == NULL) {
        printf("Cannot open file\n");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    uint8_t* buffer = malloc(sizeof(uint8_t) * size);
    fread(buffer, sizeof(uint8_t), size, file);

    load_program(&cpu, buffer, size);

    if(strcmp(argv[1], "run") == 0) {
        run(&cpu);
    }
    if(strcmp(argv[1], "decompile") == 0) {
        decompile(&cpu, size);
    }
}