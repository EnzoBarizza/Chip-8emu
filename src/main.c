#include "cpu.h"

#include <math.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <threads.h>

#define AUDIO_BUFFER_SIZE 4096
#define SAMPLE_RATE 44100

extern char* last_cycle_error;

void update_keys(cpu* cpu) {
    uint8_t chip8_keys[16] = {0x1, 0x2, 0x3, 0xC, 0x4, 0x5, 0x6, 0xD, 0x7, 0x8, 0x9, 0xE, 0xA, 0x0, 0xB, 0xF};
    int32_t raylib_keys[16] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_A, KEY_S, KEY_D, KEY_F, KEY_Z, KEY_X, KEY_C, KEY_V};

    bool old_state[16] = {};
    memcpy(&old_state, cpu->keys_pressed, sizeof(bool) * 16);

    PollInputEvents();

    for(uint8_t i = 0; i < 16; i++) {
        cpu->keys_pressed[i] = false;
    }

    for(uint8_t i = 0; i < 16; i++) {
        cpu->keys_pressed[chip8_keys[i]] = IsKeyDown(raylib_keys[i]);
    }

    if(cpu->halted) {
        for(uint8_t i = 0; i < 16; i++) {
            if(old_state[chip8_keys[i]] != cpu->keys_pressed[chip8_keys[i]]) {
                cpu->halted = false;
                cpu->key_x = chip8_keys[i];
            }
        }
    }
}

void run(cpu* cpu) {
    InitWindow(640, 320, "Chip-8 emu");
    
    SetAudioStreamBufferSizeDefault(AUDIO_BUFFER_SIZE);
    InitAudioDevice();

    float audio_buffer[AUDIO_BUFFER_SIZE] = {0};

    AudioStream audio_stream = LoadAudioStream(SAMPLE_RATE, 32, 1);
    SetAudioStreamPan(audio_stream, 0.0f);
    SetAudioStreamVolume(audio_stream, 0.3f);

    int32_t sine_freq = 200;
    int32_t sine_index = 0;
    double sine_stime = 0.0;

    uint8_t code = CONTINUE_CYCLE;

    while(code == CONTINUE_CYCLE) {
        update_keys(cpu);
        code = cycle(cpu);

        if(IsAudioStreamProcessed(audio_stream)) {
            for(uint32_t i = 0; i < AUDIO_BUFFER_SIZE; i++) {
                uint32_t wavelength = SAMPLE_RATE/sine_freq;
                audio_buffer[i] = sin(2*PI*sine_index/wavelength);
                sine_index++;

                if(sine_index >= wavelength) {
                    sine_index = 0;
                    sine_stime = GetTime();
                }
            }

            UpdateAudioStream(audio_stream, audio_buffer, AUDIO_BUFFER_SIZE);
        }

        if(cpu->ST != 0) {
            if(!IsAudioStreamPlaying(audio_stream)) {
                PlayAudioStream(audio_stream);
            }
        } else if(IsAudioStreamPlaying(audio_stream)) {
            StopAudioStream(audio_stream);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        for(int y = 0; y < 32; y++) {
            for(int x = 0; x < 64; x++) {
                if(cpu->screen_buffer[y][x] != 0)
                    DrawRectangle(x*10, y*10, 10, 10, RAYWHITE);
            }
        }
        EndDrawing();
    }

    printf("CODE = %d\n", code);

    if(code == ERROR_CODE) {
        printf("ERROR: %s\n", last_cycle_error);
    }

    UnloadAudioStream(audio_stream);
    CloseAudioDevice();
    CloseWindow();
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