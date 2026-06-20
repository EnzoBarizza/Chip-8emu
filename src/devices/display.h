#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <SDL3/SDL.h>

int init_display_device();
int SDLCALL graphical_main(int argc, char *argv[]);
SDL_AppResult SDLCALL _internal_init_display_device(void **appstate, int argc, char *argv[]);
SDL_AppResult SDLCALL _iterate_callback(void *appstate);
SDL_AppResult SDLCALL _event_callback(void *appstate, SDL_Event *event);
void SDLCALL _quit_callback(void *appstate, SDL_AppResult result);
#endif