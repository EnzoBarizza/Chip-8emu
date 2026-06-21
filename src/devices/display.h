#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <stdint.h>

int init_display_device();
void run_frame(uint8_t screen_buffer[32][64]);

#endif