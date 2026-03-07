# Changelog

## v2.0.0 — Refactored Multi-File Project

### Structure

- Split monolithic `main.cpp` into `Screen`, `Particle`, and `Swarm` classes under `src/`.
- Moved SDL basics demo to `examples/sdl-basics.cpp`.
- Added headless `tests/smoke_test.cpp` with PPM frame output.
- Added `tools/run_smoke_test.sh` and `tools/run_valgrind.sh` helper scripts.

### Safety & Idiom

- **RAII everywhere** — `std::vector<uint32_t>` replaces raw `new Uint32[]`; Screen destructor frees all SDL resources.
- **Move semantics** — Screen implements move constructor and move assignment (Rule of Five); copy is deleted.
- **No manual memory** — zero `new`/`delete` in the codebase.
- **Namespace** — all classes live inside `particle_sim`.
- **Member prefix** — private data members use `m_` convention.

### Build

- CMake now builds a `particle-core` static library shared by the app and the smoke test.
- Strict warnings: `-Wall -Wextra -Wpedantic -Werror`.
- SDL2 detection via `find_package` with `pkg-config` fallback.

### Behavior (unchanged)

- 5000 particles explode from center with polar velocities.
- 3×3 box blur produces a glow effect.
- Sine-wave color cycling based on elapsed time.

## v1.0.0 — Original Course Code

Single-file implementation following John Purcell's "Advanced C++" Udemy course.
