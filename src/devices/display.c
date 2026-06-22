#include <raylib.h>

#include "display.h"
#include <stdint.h>

int init_display_device() {
    InitWindow(640, 320, "Chip-8 emu");

    return 0;
};

void run_frame(uint8_t screen_buffer[32][64]) {
    PollInputEvents();
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