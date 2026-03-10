cmake_minimum_required(VERSION 3.10)
project(particle-fire-simulation LANGUAGES CXX)

# ── C++ Standard ──────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)          # Use -std=c++17, not -std=gnu++17

# ── Strict Compiler Warnings ─────────────────────────────────────────────────
# -Wall -Wextra -Wpedantic catch most issues.
# -Werror is enabled for CI/test builds; disable locally if needed via
#   cmake -DCMAKE_CXX_FLAGS="" ..
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# ── Find SDL2 ─────────────────────────────────────────────────────────────────
# Tries CMake's find_package first (works with vcpkg, Homebrew, apt packages).
# Falls back to pkg-config if find_package fails.
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
endif()
include_directories(${SDL2_INCLUDE_DIRS})

# ── Source library (shared by app and tests) ──────────────────────────────────
# Building the core sources as a static library avoids compiling them twice
# (once for the app, once for the test).
add_library(particle-core STATIC
    src/Screen.cpp
    src/Particle.cpp
    src/Swarm.cpp
)
target_include_directories(particle-core PUBLIC src)
target_link_libraries(particle-core PUBLIC ${SDL2_LIBRARIES})

# ── Main application ─────────────────────────────────────────────────────────
add_executable(particle-fire src/main.cpp)
target_link_libraries(particle-fire PRIVATE particle-core)

# ── SDL basics example ────────────────────────────────────────────────────────
add_executable(sdl-basics examples/sdl-basics.cpp)
target_link_libraries(sdl-basics PRIVATE ${SDL2_LIBRARIES})

# ── Main test example ────────────────────────────────────────────────────────
add_executable(main-test examples/main_test.cpp)
target_link_libraries(main-test PRIVATE particle-core)

# ── Smoke test ────────────────────────────────────────────────────────────────
add_executable(smoke-test tests/smoke_test.cpp)
target_link_libraries(smoke-test PRIVATE particle-core)

# ── Build info ────────────────────────────────────────────────────────────────
message(STATUS "SDL2 include: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2 libs:    ${SDL2_LIBRARIES}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")

