#include "display.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

int init_display_device() {
    return SDL_RunApp(NULL, NULL, graphical_main, NULL);
}

int SDLCALL graphical_main(int argc, char *argv[]) {
    return SDL_EnterAppMainCallbacks(argc, argv,
         _internal_init_display_device,
         _iterate_callback,
         _event_callback,
         _quit_callback
        );
}

SDL_AppResult SDLCALL _internal_init_display_device(void **appstate, int argc, char *argv[]) {
    SDL_SetAppMetadata("CHIP-8 EMU", "0.0.0", "enzobarizza.chip8.emu");  

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!SDL_CreateWindowAndRenderer("CHIP-8 EMU", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, 800, 600, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDLCALL _iterate_callback(void *appstate) {

}

SDL_AppResult SDLCALL _event_callback(void *appstate, SDL_Event *event) {

}

void SDLCALL _quit_callback(void *appstate, SDL_AppResult result) {

}