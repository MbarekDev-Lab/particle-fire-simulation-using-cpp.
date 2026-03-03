cmake_minimum_required(VERSION 3.10)
project(particle-fire-simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include SDL2 directories
include_directories(${SDL2_INCLUDE_DIRS})

# Create executable
add_executable(particle-fire particle-fire.cpp)

# Link SDL2 libraries
target_link_libraries(particle-fire ${SDL2_LIBRARIES})

# Print build information
message(STATUS "SDL2 include: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2 libs: ${SDL2_LIBRARIES}")
