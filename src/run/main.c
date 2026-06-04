#include <stdlib.h>
#include <time.h>
#include "chip8.h"

extern char* last_cycle_error;

int main(void) {
    srand(time(NULL));

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