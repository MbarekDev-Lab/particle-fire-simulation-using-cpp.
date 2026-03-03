//============================================================================
// Name        : SDL.cpp
// Author      : M'Barek Benraiss
// Description : Simple SDL Window and Renderer Example
// Purpose     : Learning SDL2 basics - window creation, rendering, events
//============================================================================

#include <iostream>
#include <SDL.h>

using namespace std;

int main()
{
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    cout << "Starting SDL Window Example..." << endl;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL initialization failed: " << SDL_GetError() << endl;
        return 1;
    }

    cout << "✓ SDL initialized" << endl;

    // Create window
    SDL_Window *window = SDL_CreateWindow(
        "SDL Window Example",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        cerr << "Window creation failed: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    cout << "✓ Window created (800x600)" << endl;

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        cerr << "Renderer creation failed: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    cout << "✓ Renderer created" << endl;
    cout << "\nWindow is open. Close it to exit..." << endl;

    // Event loop
    bool running = true;
    SDL_Event event;

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    cout << "\nQuit event received" << endl;
                    break;

                case SDL_KEYDOWN:
                    cout << "Key pressed: " << event.key.keysym.sym << endl;
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
            }
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
        SDL_RenderClear(renderer);

        // Draw a white rectangle
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White
        SDL_Rect rect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100, 200, 200};
        SDL_RenderFillRect(renderer, &rect);

        // Draw a red circle (as rectangle for simplicity)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red
        SDL_Rect circle = {SCREEN_WIDTH / 4 - 30, SCREEN_HEIGHT / 4 - 30, 60, 60};
        SDL_RenderFillRect(renderer, &circle);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    cout << "Cleaning up resources..." << endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    cout << "✓ SDL closed successfully" << endl;

    return 0;
}
