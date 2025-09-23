cmake_minimum_required(VERSION 3.10)
project(Survive)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required packages
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
include_directories(${CMAKE_SOURCE_DIR}/include/Core)
include_directories(${CMAKE_SOURCE_DIR}/include/Systems)
include_directories(${CMAKE_SOURCE_DIR}/include/Interface)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/lib/nlohmann)

# Add UI Framework submodule
add_subdirectory(extracted_libs/ui_framework)
include_directories(${CMAKE_SOURCE_DIR}/extracted_libs/ui_framework/include)

# Source files
set(SOURCES
    src/Core/Game.cpp
    src/Core/Event.cpp
    src/Core/Controller.cpp
    src/Core/SignalHandler.cpp
    src/Core/Inventory.cpp
    src/Core/Building.cpp
    src/Core/BaseManager.cpp
    src/Core/BaseBuildingController.cpp
    src/Core/View.cpp
    src/Core/Card.cpp
    src/Systems/DataManager.cpp
    src/Systems/SaveManager.cpp
    src/Systems/SDLManager.cpp
    src/Systems/ImGuiManager.cpp
    src/Systems/CraftingSystem.cpp
    src/Systems/TechTreeSystem.cpp
    src/Systems/GameDataValidator.cpp
    src/Interface/GameInputHandler.cpp
    # UI components now handled by ui_framework, except game-specific ones
    src/Interface/ui/UIInventoryContainer.cpp
    src/Interface/ui/UICraftingPanel.cpp
    # Tech tree components moved to ui_framework
    src/Interface/editor/GameData.cpp
    src/Interface/editor/GameEditor.cpp
    src/Interface/editor/ConsoleEditor.cpp
    src/main.cpp
)

# Create executable
add_executable(Survive ${SOURCES})

# Link libraries
target_link_libraries(Survive UIFramework SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Create library for testing (excludes main.cpp)
set(LIB_SOURCES
    src/Core/Game.cpp
    src/Core/Event.cpp
    src/Core/Controller.cpp
    src/Core/SignalHandler.cpp
    src/Core/Inventory.cpp
    src/Core/Building.cpp
    src/Core/BaseManager.cpp
    src/Core/View.cpp
    src/Core/Card.cpp
    src/Core/BaseBuildingController.cpp
    src/Systems/DataManager.cpp
    src/Systems/SaveManager.cpp
    src/Systems/ImGuiManager.cpp
    src/Systems/CraftingSystem.cpp
    src/Systems/TechTreeSystem.cpp
    src/Systems/GameDataValidator.cpp
    src/Interface/GameInputHandler.cpp
    # UI components now handled by ui_framework, except game-specific ones
    src/Interface/ui/UIInventoryContainer.cpp
    src/Interface/ui/UICraftingPanel.cpp
    # Tech tree components moved to ui_framework
    src/Interface/editor/GameData.cpp
    src/Interface/editor/GameEditor.cpp
    src/Interface/editor/ConsoleEditor.cpp
)

add_library(SurviveLib STATIC ${LIB_SOURCES})
target_link_libraries(SurviveLib UIFramework SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# FocusableButtonTest is now provided by ui_framework submodule

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
        tests/test_base_building.cpp
        tests/test_ui_container.cpp
        tests/test_ui_virtualization.cpp
        tests/test_ui_integration.cpp
        tests/test_tech_tree.cpp
    )
    
    # Create test executable
    add_executable(SurviveTests ${TEST_SOURCES})
    target_link_libraries(SurviveTests SurviveLib UIFramework SDL2::SDL2 SDL2_ttf::SDL2_ttf)
    
    # Add compile flags for threading support (required for threading tests)
    target_compile_options(SurviveTests PRIVATE -pthread)
    target_link_options(SurviveTests PRIVATE -pthread)
    
    # Create custom test target
    add_custom_target(survive_tests
        COMMAND SurviveTests
        DEPENDS SurviveTests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running Survive Catch2 tests"
    )
    
    message(STATUS "Catch2 tests enabled - use 'make survive_tests' to run tests")
else()
    message(STATUS "Catch2 not available - tests disabled")
    message(STATUS "To enable tests, ensure ${CMAKE_SOURCE_DIR}/lib/catch2/catch.hpp exists")
endif()

# Tech Tree JSON Test
add_executable(TechTreeJSONTest
    examples/tech_tree_json_test.cpp
)
target_link_libraries(TechTreeJSONTest SurviveLib UIFramework ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
target_include_directories(TechTreeJSONTest PRIVATE include lib/nlohmann ${SDL2_INCLUDE_DIRS})

# Game data validation tool
add_executable(GameDataValidationTool
    examples/game_data_validation_tool.cpp
    src/Systems/GameDataValidator.cpp
)
target_link_libraries(GameDataValidationTool ${CMAKE_DL_LIBS})
target_include_directories(GameDataValidationTool PRIVATE include lib/nlohmann)
add_executable(TechTreeExample examples/tech_tree_example.cpp)
target_link_libraries(TechTreeExample SurviveLib UIFramework SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Add UILabel tech tree test
add_executable(TechTreeUITest examples/tech_tree_ui_test.cpp)
target_link_libraries(TechTreeUITest SurviveLib UIFramework SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Add layout demo
add_executable(TechTreeLayoutDemo examples/tech_tree_layout_demo.cpp)
target_link_libraries(TechTreeLayoutDemo SurviveLib UIFramework SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Add console demo
add_executable(TechTreeConsoleDemo examples/tech_tree_console_demo.cpp)
target_link_libraries(TechTreeConsoleDemo SurviveLib)

# Add tech tree crafting integration test
add_executable(TechTreeCraftingIntegrationTest examples/tech_tree_crafting_integration_test.cpp)
target_link_libraries(TechTreeCraftingIntegrationTest SurviveLib UIFramework SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Add simple integration test (avoids SimpleGameController dependencies)
add_executable(SimpleIntegrationTest examples/simple_integration_test.cpp)
target_link_libraries(SimpleIntegrationTest SurviveLib UIFramework SDL2::SDL2 SDL2_ttf::SDL2_ttf)
