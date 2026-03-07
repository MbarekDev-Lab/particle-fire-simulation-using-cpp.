// ============================================================================
// sdl-basics.cpp — SDL2 Basics: Window, Renderer, and Event Loop
//
// Author      : M'Barek Benraiss
// Description : Standalone example demonstrating core SDL2 concepts
//
// This is a self-contained tutorial (not part of the particle simulation)
// covering these SDL2 fundamentals:
//   1. SDL_Init / SDL_Quit lifecycle
//   2. Window creation with SDL_CreateWindow
//   3. Hardware-accelerated renderer with VSync
//   4. Event polling (keyboard + quit)
//   5. Basic shape drawing with SDL_RenderFillRect
//   6. Proper resource cleanup in reverse creation order
//
// Build: cmake --build build --target sdl-basics
// Run:   ./build/sdl-basics
// ============================================================================

#include <SDL.h>
#include <iostream>

int main() {
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;

    // --- Step 1: Initialize SDL video subsystem ---
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // --- Step 2: Create a window ---
    SDL_Window* window =
        SDL_CreateWindow("SDL Basics Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // --- Step 3: Create a renderer ---
    // SDL_RENDERER_PRESENTVSYNC syncs with the monitor refresh rate
    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- Step 4: Event loop ---
    // Without this loop the window would flash open and immediately close.
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        // --- Step 5: Render ---
        // Clear to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // White rectangle in the center
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect centerRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100, 200, 200};
        SDL_RenderFillRect(renderer, &centerRect);

        // Small red square in the top-left quadrant
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect redRect = {SCREEN_WIDTH / 4 - 30, SCREEN_HEIGHT / 4 - 30, 60, 60};
        SDL_RenderFillRect(renderer, &redRect);

        SDL_RenderPresent(renderer);
    }

    // --- Step 6: Cleanup in reverse creation order ---
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
