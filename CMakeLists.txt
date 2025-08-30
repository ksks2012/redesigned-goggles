cmake_minimum_required(VERSION 3.10)
project(Survive)

# Find SDL2 and SDL2_ttf
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/SDLManager.cpp
    src/Inventory.cpp
    src/View.cpp
    src/Controller.cpp
    src/Game.cpp
    src/main.cpp
    src/Event.cpp
)

# Create executable
add_executable(Survive ${SOURCES})

# Link libraries
target_link_libraries(Survive SDL2::SDL2 SDL2_ttf::SDL2_ttf)