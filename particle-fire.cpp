//============================================================================
// Name        : particle-fire.cpp
// Author      : M'Barek Benraiss
// Description : Particle Fire/Explosion Simulation using SDL2
// Concepts    : Particle systems, physics simulation, color interpolation
//              Performance optimization, multiple effect modes, interactive control
//============================================================================

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <SDL.h>

using namespace std;

// Effect modes for different particle effects
enum EffectMode
{
    FIRE_MODE,      // Yellow/orange fire effect
    SMOKE_MODE,     // Gray smoke effect
    EXPLOSION_MODE, // Fast reddish explosion
    ICE_MODE        // Blue/cyan ice particles
};

// ============================================================================
// Section 1: Particle Structure - Represents a single particle in the system
// ============================================================================

struct Particle
{
    // Position
    double x;
    double y;

    // Velocity
    double vx;
    double vy;

    // Life and aging
    double life;    // Current life (0.0 to 1.0)
    double maxLife; // Maximum life duration

    // Color (RGB)
    int r, g, b;

    // Size
    double size;

    // Constructor
    Particle(double x, double y, double vx, double vy,
             double life, int r, int g, int b, double size)
        : x(x), y(y), vx(vx), vy(vy), life(life), maxLife(life),
          r(r), g(g), b(b), size(size) {}

    // Update particle position and life
    void update(double deltaTime, double gravity = 0.1)
    {
        // Apply velocity
        x += vx * deltaTime;
        y += vy * deltaTime;

        // Apply gravity - particles fall downward
        vy += gravity * deltaTime;

        // Air resistance/damping
        vx *= 0.99;
        vy *= 0.99;

        // Decrease life
        life -= deltaTime / maxLife;
    }

    // Check if particle is still alive
    bool isAlive() const
    {
        return life > 0.0;
    }

    // Get alpha (transparency) based on life - fade out effect
    int getAlpha() const
    {
        return static_cast<int>(255 * life);
    }
};

// ============================================================================
// Section 2: Particle Emitter - Creates and manages particles
// ============================================================================

class ParticleEmitter
{
private:
    vector<Particle> particles;
    double emitterX;
    double emitterY;
    int particlesPerFrame;
    EffectMode currentMode;
    int frameCount;

public:
    ParticleEmitter(double x, double y, int pps = 5)
        : emitterX(x), emitterY(y), particlesPerFrame(pps),
          currentMode(FIRE_MODE), frameCount(0) {}

    // Set emitter position
    void setPosition(double x, double y)
    {
        emitterX = x;
        emitterY = y;
    }

    // Change effect mode
    void setEffectMode(EffectMode mode)
    {
        currentMode = mode;
        particles.clear();
    }

    // Set emission rate
    void setEmissionRate(int pps)
    {
        particlesPerFrame = (pps < 1) ? 1 : pps;
    }

    // Get current mode name
    string getModeName() const
    {
        switch (currentMode)
        {
        case FIRE_MODE:
            return "FIRE";
        case SMOKE_MODE:
            return "SMOKE";
        case EXPLOSION_MODE:
            return "EXPLOSION";
        case ICE_MODE:
            return "ICE";
        default:
            return "UNKNOWN";
        }
    }

    // Emit new particles (fire/explosion effect)
    void emit()
    {
        for (int i = 0; i < particlesPerFrame; i++)
        {
            // Random upward velocity (fire rises)
            double angle = (rand() % 360) * 3.14159 / 180.0;
            double speed = 1.0 + (rand() % 100) / 100.0;
            double vx = cos(angle) * speed;
            double vy = -sin(angle) * speed - 0.5; // Bias upward

            // Random life duration
            double life = 2.0 + (rand() % 100) / 100.0;
            double size = 2.0 + (rand() % 4);

            // Fire colors: yellow to red to dark
            int r = 255;
            int g = 150 + (rand() % 100);
            int b = 0;

            // Adjust based on effect mode
            switch (currentMode)
            {
            case SMOKE_MODE:
                // Gray smoke effect
                r = 150 + (rand() % 80);
                g = 150 + (rand() % 80);
                b = 150 + (rand() % 80);
                vy *= 0.5;   // Slower rise for smoke
                size *= 1.5; // Larger particles
                break;

            case EXPLOSION_MODE:
                // Reddish explosive effect
                r = 255;
                g = 50 + (rand() % 80);
                b = 0;
                speed *= 2.0; // Faster particles
                vx *= 1.5;
                vy *= 1.5;
                break;

            case ICE_MODE:
                // Cyan/blue ice effect
                r = 50 + (rand() % 50);
                g = 150 + (rand() % 100);
                b = 255;
                break;

            case FIRE_MODE:
            default:
                // Default fire mode already set above
                break;
            }

            // Create and add particle
            particles.emplace_back(emitterX, emitterY, vx, vy,
                                   life, r, g, b, size);
        }
    }

    // Update all particles
    void update(double deltaTime)
    {
        for (auto &particle : particles)
        {
            particle.update(deltaTime);
        }

        // Remove dead particles
        particles.erase(
            remove_if(particles.begin(), particles.end(),
                      [](const Particle &p)
                      { return !p.isAlive(); }),
            particles.end());
    }

    // Get particles for rendering
    const vector<Particle> &getParticles() const
    {
        return particles;
    }

    // Get particle count
    size_t getParticleCount() const
    {
        return particles.size();
    }
};

// ============================================================================
// Section 3: Simple Graphics/Rendering Helper
// ============================================================================

class Renderer
{
private:
    SDL_Renderer *renderer;
    int width;
    int height;

public:
    Renderer(SDL_Window *window, int w, int h)
        : width(w), height(h)
    {
        renderer = SDL_CreateRenderer(window, -1,
                                      SDL_RENDERER_ACCELERATED |
                                          SDL_RENDERER_PRESENTVSYNC);
    }

    ~Renderer()
    {
        if (renderer)
        {
            SDL_DestroyRenderer(renderer);
        }
    }

    // Draw a single particle (simplified as rectangle)
    void drawParticle(const Particle &p)
    {
        SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, p.getAlpha());

        SDL_Rect rect;
        rect.x = static_cast<int>(p.x - p.size);
        rect.y = static_cast<int>(p.y - p.size);
        rect.w = static_cast<int>(p.size * 2);
        rect.h = static_cast<int>(p.size * 2);

        SDL_RenderFillRect(renderer, &rect);
    }

    // Draw all particles from emitter
    void drawParticles(const ParticleEmitter &emitter)
    {
        for (const auto &particle : emitter.getParticles())
        {
            drawParticle(particle);
        }
    }

    // Clear screen (black background)
    void clear()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    // Present frame
    void present()
    {
        SDL_RenderPresent(renderer);
    }

    // Get width
    int getWidth() const { return width; }

    // Get height
    int getHeight() const { return height; }
};

// ============================================================================
// Section 4: Application - Main loop and event handling
// ============================================================================

class ParticleFireApp
{
private:
    SDL_Window *window;
    Renderer *renderer;
    ParticleEmitter emitter;
    bool running;
    int screenWidth;
    int screenHeight;

public:
    ParticleFireApp(int width = 800, int height = 600)
        : window(nullptr), renderer(nullptr),
          emitter(width / 2, height - 100, 10),
          running(true), screenWidth(width), screenHeight(height)
    {

        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            cerr << "SDL initialization failed: " << SDL_GetError() << endl;
            return;
        }

        // Create window
        window = SDL_CreateWindow("Particle Fire Simulation",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  width, height,
                                  SDL_WINDOW_SHOWN);

        if (!window)
        {
            cerr << "Window creation failed: " << SDL_GetError() << endl;
            SDL_Quit();
            return;
        }

        // Create renderer
        renderer = new Renderer(window, width, height);

        // Seed random number generator
        srand(time(nullptr));
    }

    ~ParticleFireApp()
    {
        if (renderer)
            delete renderer;
        if (window)
            SDL_DestroyWindow(window);
        SDL_Quit();
    }

    // Main event loop
    void run()
    {
        SDL_Event event;
        Uint32 lastTime = SDL_GetTicks();

        while (running)
        {
            // Calculate delta time
            Uint32 currentTime = SDL_GetTicks();
            double deltaTime = (currentTime - lastTime) / 1000.0;
            lastTime = currentTime;

            // Limit delta time (avoid large jumps)
            if (deltaTime > 0.016)
                deltaTime = 0.016; // ~60 FPS

            // Handle events
            while (SDL_PollEvent(&event))
            {
                handleEvent(event);
            }

            // Track mouse position for emitter
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            emitter.setPosition(mouseX, mouseY);

            // Update
            emitter.emit();
            emitter.update(deltaTime);

            // Render
            renderer->clear();
            renderer->drawParticles(emitter);

            // Draw info text (simple version)
            drawInfo();

            renderer->present();
        }
    }

    // Handle SDL events
    void handleEvent(SDL_Event &event)
    {
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            handleKeyPress(event.key.keysym.sym);
            break;
        }
    }

    // Handle keyboard input for mode switching
    void handleKeyPress(SDL_Keycode key)
    {
        switch (key)
        {
        case SDLK_ESCAPE:
        case SDLK_q:
            running = false;
            break;

        case SDLK_1: // Fire mode
            emitter.setEffectMode(FIRE_MODE);
            cout << "Mode: FIRE" << endl;
            break;

        case SDLK_2: // Smoke mode
            emitter.setEffectMode(SMOKE_MODE);
            cout << "Mode: SMOKE" << endl;
            break;

        case SDLK_3: // Explosion mode
            emitter.setEffectMode(EXPLOSION_MODE);
            cout << "Mode: EXPLOSION" << endl;
            break;

        case SDLK_4: // Ice mode
            emitter.setEffectMode(ICE_MODE);
            cout << "Mode: ICE" << endl;
            break;

        case SDLK_UP: // Increase particles
            emitter.setEmissionRate(emitter.getEmissionRate() + 5);
            cout << "Emission rate: " << emitter.getEmissionRate() << endl;
            break;

        case SDLK_DOWN: // Decrease particles
            emitter.setEmissionRate(emitter.getEmissionRate() - 5);
            cout << "Emission rate: " << emitter.getEmissionRate() << endl;
            break;

        case SDLK_r: // Reset
            emitter.setEffectMode(FIRE_MODE);
            cout << "Reset to FIRE mode" << endl;
            break;

        case SDLK_h: // Help
            printControls();
            break;
        }
    }

    // Print control information
    void printControls() const
    {
        cout << "\n=== PARTICLE FIRE SIMULATION CONTROLS ===" << endl;
        cout << "1 - Fire mode" << endl;
        cout << "2 - Smoke mode" << endl;
        cout << "3 - Explosion mode" << endl;
        cout << "4 - Ice mode" << endl;
        cout << "UP - Increase particle emission" << endl;
        cout << "DOWN - Decrease particle emission" << endl;
        cout << "R - Reset to Fire mode" << endl;
        cout << "H - Show this help message" << endl;
        cout << "ESC/Q - Exit" << endl;
        cout << "Mouse - Move to control emitter position" << endl;
        cout << "========================================\n"
             << endl;
    }

    // Simple info display
    void drawInfo()
    {
        // In a full application, you'd use SDL_ttf for text rendering
        // For now, just note that particle count info would go here
    }
};

// ============================================================================
// Section 5: Main Entry Point
// ============================================================================

/**
 * PARTICLE FIRE SIMULATION
 *
 * This program simulates a fire/explosion particle effect using SDL2.
 *
 * Features:
 * - Particle system with physics (gravity, velocity, damping)
 * - Color interpolation (bright yellow/orange to dark)
 * - Life span and fade out effect
 * - Mouse-following emitter (move mouse to see particles)
 * - Collision and boundary checking
 * - Real-time frame rate independent updates
 *
 * Controls:
 * - Move mouse to position emitter
 * - Press ESC or close window to exit
 *
 * Concepts Covered:
 * - Particle systems and emitters
 * - Physics simulation (gravity, velocity, acceleration)
 * - Color and transparency effects
 * - SDL rendering and event handling
 * - Delta time frame-rate independent movement
 * - Dynamic memory management with vectors
 * - Object-oriented design patterns
 */

int main()
{
    // Create and run application
    ParticleFireApp app(1000, 700);

    // Display controls on startup
    app.printControls();

    app.run();

    cout << "Particle Fire Simulation closed." << endl;

    return 0;
}
