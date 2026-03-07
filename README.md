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

## Expected Output

```
$ mkdir -p build && cd build && cmake .. && make
-- SDL2 include: /opt/homebrew/Cellar/sdl2/2.32.10/include;/opt/homebrew/Cellar/sdl2/2.32.10/include/SDL2
-- SDL2 libs:    SDL2::SDL2
-- C++ standard: 17
-- Configuring done (0.1s)
-- Generating done (0.0s)
[ 40%] Built target particle-core
[ 60%] Built target particle-fire
[ 80%] Built target sdl-basics
[100%] Built target smoke-test

$ mkdir -p out && ./smoke-test
=== Particle Fire Smoke Test ===
Test 1: Screen init (64x64)... OK
  (RAII destructor: OK)
Test 2: setPixel + buffer read... OK (127 lit pixels)
Test 3: Out-of-bounds setPixel... OK (no crash)
Test 4: boxBlur changes pixel buffer... OK (127 → 4096 lit pixels)
Test 5: Write out/frame0.ppm... OK
Test 6: Particle initRandom + update + respawn... OK
Test 7: Swarm creation and update... OK
Test 8: clear() resets buffer... OK

=== ALL 17 CHECKS PASSED ===
```

Or use the helper script:

```bash
./tools/run_smoke_test.sh
```

## Author

Created by M'Barek Benraiss. 
