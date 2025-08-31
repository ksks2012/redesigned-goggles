cmake_minimum_required(VERSION 3.10)
project(Survive)

# Find SDL2 and SDL2_ttf
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)

# Source files
set(SOURCES
    src/SDLManager.cpp
    src/Inventory.cpp
    src/View.cpp
    src/Controller.cpp
    src/Game.cpp
    src/SaveManager.cpp
    src/CraftingSystem.cpp
    src/main.cpp
    src/Event.cpp
    src/ui/UIComponent.cpp
    src/ui/UIButton.cpp
    src/ui/UICard.cpp
    src/ui/UITooltip.cpp
    src/ui/UICraftingPanel.cpp
    src/ImGuiManager.cpp
    src/editor/GameData.cpp
    src/editor/GameEditor.cpp
    src/editor/ConsoleEditor.cpp
)

# Create executable
add_executable(Survive ${SOURCES})

# Link libraries
target_link_libraries(Survive SDL2::SDL2 SDL2_ttf::SDL2_ttf)