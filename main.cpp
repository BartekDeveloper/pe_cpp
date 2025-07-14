#include "main.hpp"
#include <stdio.h>
#define SDL_INCLUDE_VULKAN
#include <SDL3/SDL.h>

int main(void) {
    puts("initializing SDL");
    SDL_Init(SDL_INIT_VIDEO);
    
    puts("SDL initialized");
    
    SDL_Quit();
    puts("SDL quit");

    return 0;
}

