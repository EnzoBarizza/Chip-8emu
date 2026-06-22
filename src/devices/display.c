#include <raylib.h>

#include "display.h"
#include <stdint.h>

void init_display_device() {
    InitWindow(640, 320, "Chip-8 emu");
    InitAudioDevice();
};

void run_frame(uint8_t screen_buffer[32][64]) {
    BeginDrawing();
    ClearBackground(BLACK);
    for(int y = 0; y < 32; y++) {
        for(int x = 0; x < 64; x++) {
            if(screen_buffer[y][x] != 0)
                DrawRectangle(x*10, y*10, 10, 10, RAYWHITE);
        }
    }
    EndDrawing();
}

void deinit_display_device() {
    CloseWindow();
    CloseAudioDevice();
}