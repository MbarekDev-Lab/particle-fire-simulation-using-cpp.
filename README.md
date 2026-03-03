# Particle Fire Simulation Using C++

An interactive **particle fire/explosion simulation** using SDL2 graphics library. Move your mouse to emit particles and watch them simulate realistic fire behavior with physics.

## Features

- **Particle System**: Dynamic creation and management of thousands of particles
- **Physics Simulation**: 
  - Gravity affecting particles over time
  - Velocity and acceleration
  - Air resistance (damping)
  - Smooth interpolated motion
- **Visual Effects**:
  - Fire-like color gradients (yellow → orange → red → dark)
  - Fade-out effect based on particle lifetime
  - Real-time rendering with SDL2
- **Interactive**: Follow your mouse cursor with particle emissions
- **Performance**: Frame-rate independent updates using delta time

## Building

### Prerequisites

1. **SDL2** installed via Homebrew (macOS):
   ```bash
   brew install sdl2
   ```

2. **CMake** (for building):
   ```bash
   brew install cmake
   ```

3. **C++ Compiler** (Clang on macOS, g++ on Linux/Windows):
   - macOS: Built-in with Xcode Command Line Tools
   - Linux: `sudo apt install g++`
   - Windows: Visual Studio or MinGW

### Build Instructions

```bash
# Clone repository
git clone https://github.com/MbarekDev-Lab/particle-fire-simulation-using-cpp.git
cd particle-fire-simulation-using-cpp

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run
./particle-fire
```

### Expected Output
A window opens showing a black background. Move your mouse around the screen to emit particles that create a fire-like effect.

## How It Works

### Architecture

```
┌─────────────────────────────────────┐
│   ParticleFireApp (Main Application)│
├─────────────────────────────────────┤
│  • Event handling (SDL_PollEvent)   │
│  • Main update/render loop          │
│  • Window management                │
└─────────────┬───────────────────────┘
              │
         creates
              │
         ┌────▼──────────────────────────┐
         │  ParticleEmitter             │
         ├────────────────────────────────┤
         │  • Manages particle vector    │
         │  • Updates all particles      │
         │  • Removes dead particles     │
         │  • Emits new particles       │
         └────┬──────────────────────────┘
              │
         manages
              │
         ┌────▼──────────────────────────┐
         │  Particle (struct)            │
         ├────────────────────────────────┤
         │  • Position (x, y)            │
         │  • Velocity (vx, vy)          │
         │  • Color (r, g, b)            │
         │  • Life (0.0 to 1.0)          │
         │  • Size and physics           │
         └────┬──────────────────────────┘
              │
         uses
              │
         ┌────▼──────────────────────────┐
         │  Renderer (SDL wrapper)       │
         ├────────────────────────────────┤
         │  • SDL_Renderer abstraction   │
         │  • Drawing primitives        │
         │  • Screen management         │
         └──────────────────────────────┘
```

### Particle Lifecycle

1. **Emission**: `ParticleEmitter::emit()` creates particles at cursor position
2. **Update**: `Particle::update()` applies physics each frame
   - Position += Velocity * deltaTime
   - Velocity += Gravity * deltaTime
   - All velocities *= damping factor
   - Life decreases over time
3. **Render**: `Renderer::drawParticle()` displays each particle
   - Color based on type (fire = yellow/orange)
   - Alpha (transparency) based on remaining life
   - Size decreases as it ages (optional)
4. **Removal**: Dead particles (life ≤ 0) removed from vector

### Physics Implementation

Each particle is affected by:

**Gravity**:
```cpp
vy += gravity * deltaTime;  // Default: 0.1
```

**Air Resistance**:
```cpp
vx *= 0.99;  // 1% deceleration per frame
vy *= 0.99;
```

**Velocity-based Position**:
```cpp
x += vx * deltaTime;
y += vy * deltaTime;
```

### Color Interpolation

Fire particles use RGB values that create realistic flame colors:
- **Bright yellow**: (255, 200, 0) - hottest (birth)
- **Orange**: (255, 120, 0) - hot
- **Dark red**: (200, 0, 0) - cooling
- **Black**: (0, 0, 0) - dead/cooled

### Delta Time (Frame-Rate Independence)

```cpp
Uint32 currentTime = SDL_GetTicks();
double deltaTime = (currentTime - lastTime) / 1000.0;
lastTime = currentTime;

// Now update uses deltaTime:
particle.update(deltaTime);  // Same speed regardless of FPS
```

This ensures smooth animation at 30 FPS or 120 FPS.

## Controls

| Key/Input | Action |
|-----------|--------|
| **Mouse Movement** | Move emitter to follow cursor |
| **ESC** | Exit application |
| **Close Window** | Exit application |

## Code Structure

### Section 1: Particle Structure
- `struct Particle`: Individual particle data and behavior
- `update()`: Apply physics each frame
- `isAlive()`: Check if particle still visible
- `getAlpha()`: Calculate transparency for fade effect

### Section 2: ParticleEmitter
- `ParticleEmitter` class: Manages all particles
- `emit()`: Generate new particles at random velocities/colors
- `update()`: Update all particles, remove dead ones
- `getParticles()`: Return particle list for rendering

### Section 3: Renderer
- `Renderer` class: SDL2 abstraction layer
- `drawParticle()`: Render single particle as rectangle
- `drawParticles()`: Render all particles from emitter
- `clear()`: Clear screen for next frame
- `present()`: Display frame to user

### Section 4: Application
- `ParticleFireApp` class: Main application
- `run()`: Main loop (events → update → render)
- `handleEvent()`: Process keyboard/window events
- `drawInfo()`: Display statistics (prepared for text rendering)

## Customization Ideas

### Easy Modifications

1. **Change emission rate**:
   ```cpp
   emitter = ParticleEmitter(width/2, height-100, 20);  // More particles
   ```

2. **Adjust gravity**:
   ```cpp
   particle.update(deltaTime, 0.05);  // Less gravity - smoke-like
   ```

3. **Change colors** (in `emit()`):
   ```cpp
   // Blue fire effect
   int r = 0;
   int g = 150 + (rand() % 100);
   int b = 255;
   ```

4. **Particle lifetime**:
   ```cpp
   double life = 0.5 + (rand() % 100) / 100.0;  // Shorter lifespan
   ```

### Advanced Features

- **Collision Detection**: Check particle boundaries, bounce off walls
- **Wind Effect**: Add horizontal force to all particles
- **Texture-based Rendering**: Use SDL_Texture instead of rectangles
- **Text Rendering**: Add SDL_ttf library for FPS display
- **Multiple Emitters**: Create emitter array for complex effects
- **Sound Effects**: Add SDL_mixer for audio feedback
- **Particle Trails**: Store particle history for trail effect

## Performance Considerations

- **Vector Reserve**: Pre-allocate vector capacity for smoother allocation
  ```cpp
  particles.reserve(10000);  // Avoid frequent reallocations
  ```

- **Delta Time Cap**: Prevent huge jumps when frame drops
  ```cpp
  if (deltaTime > 0.016) deltaTime = 0.016;
  ```

- **Particle Limit**: Cap maximum particles to maintain FPS
  ```cpp
  if (particles.size() > 5000) return;  // Limit creation
  ```

- **Simple Rendering**: Rectangles are faster than textures for many particles

## Troubleshooting

### "SDL not found" Error
```bash
brew install sdl2
brew reinstall sdl2  # If already installed
```

### "command not found: sdl2-config"
Indicates SDL2 not properly installed. Try:
```bash
brew uninstall sdl2
brew install sdl2
```

### Build Fails with CMake Errors
```bash
rm -rf build/
mkdir build && cd build
cmake ..
make
```

### No Particles Visible / Black Screen
- Ensure window is focused
- Move your mouse around the window
- Check that SDL2 initialized successfully (check console output)

## Learning Outcomes

By studying this project, you'll learn:

✅ **Particle Systems**: How games/effects create dynamic visuals  
✅ **Physics Simulation**: Applying real-world forces in code  
✅ **Object-Oriented Design**: Organizing complex systems with classes  
✅ **SDL2 Graphics**: Cross-platform rendering library  
✅ **Performance Optimization**: Frame-rate independent updates  
✅ **Memory Management**: Dynamic allocation with vectors  
✅ **Delta Time**: Making smooth animation at any frame rate  

## References

- **SDL2 Documentation**: https://wiki.libsdl.org/
- **Particle Systems**: https://en.wikipedia.org/wiki/Particle_system
- **Physics in Games**: https://gamedevelopment.tutsplus.com/

## License

MIT License - Feel free to use and modify for learning purposes.

## Author

Created by M'Barek Benraiss as part of C++ learning progression.

---

**Ready to run?** Follow the [Building](#building) section to get started! 🔥
