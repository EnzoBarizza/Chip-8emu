#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <stdint.h>

void init_display_device();
void run_frame(uint8_t screen_buffer[32][64]);
void deinit_display_device();

#endif