# Particle Fire Simulation

A particle fire/explosion simulation using **SDL2** and direct pixel buffer manipulation in **C++17**.

Thousands of particles explode outward from the center with a 3×3 box-blur glow effect and sine-wave color cycling.

## Project Structure

```
├── CMakeLists.txt              # Build system (3 targets)
├── src/
│   ├── main.cpp                # Application entry point
│   ├── Screen.h / Screen.cpp   # RAII SDL wrapper + pixel buffers + box blur
│   ├── Particle.h / Particle.cpp # Single particle (polar velocity)
│   └── Swarm.h / Swarm.cpp     # Manages 5000 particles
├── examples/
│   └── sdl-basics.cpp          # Standalone SDL2 tutorial
├── tests/
│   └── smoke_test.cpp          # Headless smoke test (PPM output)
├── tools/
│   ├── run_smoke_test.sh       # Build + run smoke test
│   └── run_valgrind.sh         # Optional Valgrind memory check
├── docs/
│   └── CHANGELOG.md            # Change history and rationale
├── .clang-format               # Code style configuration
├── .editorconfig               # Editor settings
├── CONTRIBUTING.md             # Coding guidelines
└── README.md                   # This file
```

## Prerequisites

| Platform | Install SDL2                                                                |
| -------- | --------------------------------------------------------------------------- |
| macOS    | `brew install sdl2`                                                         |
| Ubuntu   | `sudo apt install libsdl2-dev`                                              |
| Windows  | `vcpkg install sdl2` or download from [libsdl.org](https://www.libsdl.org/) |

CMake ≥ 3.10 and a C++17 compiler are also required.

## Build & Run

```bash
# Configure + build all targets
mkdir -p build && cd build
cmake ..
make            # or: cmake --build .

# Run the simulation
./particle-fire

# Run the SDL basics example
./sdl-basics
```

## Run the Smoke Test

The smoke test validates the pixel buffer, box blur, and RAII cleanup in headless mode:

```bash
# Option A: use the provided script
./tools/run_smoke_test.sh

# Option B: manual
cd build
cmake .. && make smoke-test
mkdir -p out
./smoke-test
# → produces out/frame0.ppm
```

## Controls

| Input        | Action |
| ------------ | ------ |
| Close window | Quit   |

## Key Concepts Demonstrated

- **Pixel buffer manipulation** — writing RGBA values directly to a texture
- **RGBA8888 bit packing** — `(red << 24) | (green << 16) | (blue << 8) | 0xFF`
- **3×3 box blur** — convolution kernel averaging each pixel with its neighbors
- **Polar-to-Cartesian conversion** — `dx = speed × cos(θ)`, `dy = speed × sin(θ)`
- **Frame-rate independence** — delta time between frames drives all movement
- **RAII** — `std::vector` for buffers, destructor for SDL cleanup, no manual `new`/`delete`
- **Move semantics** — Screen supports move constructor / move assignment

## License

See [LICENSE](LICENSE).

## Controls

| Key/Input          | Action                        |
| ------------------ | ----------------------------- |
| **Mouse Movement** | Move emitter to follow cursor |
| **ESC**            | Exit application              |
| **Close Window**   | Exit application              |

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
