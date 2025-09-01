cmake_minimum_required(VERSION 3.10)
project(Survive)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Check if Catch2 header is available
set(CATCH2_HEADER "${CMAKE_SOURCE_DIR}/lib/catch2/catch.hpp")
if(EXISTS ${CATCH2_HEADER})
    set(CATCH2_AVAILABLE TRUE)
    message(STATUS "Catch2 header found at ${CATCH2_HEADER}")
else()
    set(CATCH2_AVAILABLE FALSE)
    message(STATUS "Catch2 header not found - tests disabled")
endif()

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
    src/DataManager.cpp
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
    src/GameInputHandler.cpp
)

# Create executable
add_executable(Survive ${SOURCES})

# Link libraries
target_link_libraries(Survive SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Create library for testing (excludes main.cpp)
set(LIB_SOURCES
    src/SDLManager.cpp
    src/Inventory.cpp
    src/View.cpp
    src/Controller.cpp
    src/Game.cpp
    src/SaveManager.cpp
    src/CraftingSystem.cpp
    src/DataManager.cpp
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
    src/GameInputHandler.cpp
)

add_library(SurviveLib STATIC ${LIB_SOURCES})
target_link_libraries(SurviveLib SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Add tests if Catch2 is available
if(CATCH2_AVAILABLE)
    enable_testing()
    
    # Test source files
    set(TEST_SOURCES
        tests/main.cpp
        tests/test_card.cpp
        tests/test_inventory.cpp
        tests/test_crafting_system.cpp
        tests/test_data_manager.cpp
    )
    
    # Create test executable
    add_executable(SurviveTests ${TEST_SOURCES})
    target_link_libraries(SurviveTests SurviveLib SDL2::SDL2 SDL2_ttf::SDL2_ttf)
    
    # Add compile flags for threading support (required for threading tests)
    target_compile_options(SurviveTests PRIVATE -pthread)
    target_link_options(SurviveTests PRIVATE -pthread)
    
    # Create custom test target
    add_custom_target(test_run
        COMMAND SurviveTests
        DEPENDS SurviveTests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running Catch2 tests"
    )
    
    message(STATUS "Catch2 tests enabled - use 'make test_run' to run tests")
else()
    message(STATUS "Catch2 not available - tests disabled")
    message(STATUS "To enable tests, ensure ${CMAKE_SOURCE_DIR}/lib/catch2/catch.hpp exists")
endif()