#include "Game.h"
#include "Constants.h"
#include <thread>
#include <iostream>

Game::Game() : sdlManager(), inventory(), craftingSystem(), view(sdlManager), controller(inventory, view, craftingSystem), saveManager("game_save.json") {
    // Set save and load callback functions
    controller.setSaveCallback([this]() { return this->saveGame(); });
    controller.setLoadCallback([this]() { return this->loadGame(); });
    
    // Initialize data management system
    initializeDataSystem();
    
    // Initialize editor system
    initializeEditor();
    
    // Load game data from JSON files
    if (!loadGameData()) {
        std::cout << "Game data not found or invalid, creating default data" << std::endl;
        globalDataManager->createDefaultDataFiles();
        globalDataManager->saveAllData();
        loadGameData(); // Reload the newly created data
    }
    
    // Try to load the save file; if it fails, initialize the default game
    if (!loadGame()) {
        std::cout << "Save file not found, starting a new game" << std::endl;
        initializeDefaultGame();
    }
}

void Game::run() {
    std::thread organizer([this]() { controller.organizeInventory(); });
    while (controller.isRunning()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // Handle editor input first
            if (imguiManager.handleEvent(&event)) {
                continue; // Editor consumed the event
            }
            
            // Handle game input
            controller.handleEvent(event);
        }
        
        // Update editor
        if (imguiManager.isEditorMode()) {
            imguiManager.beginFrame(sdlManager.getWindow());
            gameEditor->update();
            gameEditor->render();
            imguiManager.endFrame();
        }
        
        controller.updateView();
        
        // Render ImGui overlay
        imguiManager.render();
        
        SDL_Delay(Constants::FRAME_DELAY_MS);
    }
    
    // Automatically save when the game ends
    std::cout << "Game ended, saving..." << std::endl;
    saveGame();
    
    if (organizer.joinable()) organizer.join();
}

bool Game::saveGame() {
    return saveManager.saveGame(inventory);
}

bool Game::loadGame() {
    return saveManager.loadGame(inventory);
}

void Game::initializeDefaultGame() {
    for (const auto& card : Constants::INITIAL_CARDS) {
        inventory.addCard(card);
    }
}

void Game::initializeEditor() {
    // Create data manager for editor with game instance reference
    dataManager = std::make_unique<GameDataManager>(this);
    
    if (imguiManager.initialize(sdlManager.getWindow(), sdlManager.getRenderer())) {
        // Provide data manager to ImGuiManager
        imguiManager.setDataManager(globalDataManager.get());
        imguiManager.setGameInstance(this);
        
        // Set callback for editor mode changes to control organizeInventory
        imguiManager.setEditorModeCallback([this](bool editorMode) {
            if (editorMode) {
                controller.pauseOrganizeInventory();
            } else {
                controller.resumeOrganizeInventory();
            }
        });
        
        gameEditor = std::make_unique<GameEditor>();
        gameEditor->initialize(imguiManager);
        std::cout << "Editor system initialized. Press F1 to toggle editor mode." << std::endl;
    } else {
        std::cout << "Failed to initialize editor system" << std::endl;
    }
}

// TODO: Implement detailed editor input handling if needed
void Game::handleEditorInput(SDL_Event& event) {
}

void Game::initializeDataSystem() {
    globalDataManager = std::make_unique<DataManagement::GameDataManager>();
    std::cout << "Data management system initialized" << std::endl;
}

bool Game::loadGameData() {
    if (!globalDataManager) {
        std::cerr << "Data manager not initialized" << std::endl;
        return false;
    }
    
    // Try to load all game data
    if (!globalDataManager->loadAllData()) {
        std::cerr << "Failed to load game data files" << std::endl;
        return false;
    }
    
    // Validate the loaded data
    if (!validateGameData()) {
        std::cerr << "Game data validation failed" << std::endl;
        return false;
    }
    
    // Apply data to game systems
    globalDataManager->applyToInventory(inventory);
    globalDataManager->applyToCraftingSystem(craftingSystem);
    globalDataManager->applyToController(controller);
    
    std::cout << "Successfully loaded and applied game data" << std::endl;
    return true;
}

bool Game::saveGameData() const {
    if (!globalDataManager) {
        std::cerr << "Data manager not initialized" << std::endl;
        return false;
    }
    
    return globalDataManager->saveAllData();
}

bool Game::validateGameData() const {
    if (!globalDataManager) {
        std::cerr << "Data manager not initialized" << std::endl;
        return false;
    }
    
    DataManagement::ValidationResult result = globalDataManager->validateAll();
    
    if (!result.isValid) {
        std::cerr << "Data validation failed:\n" << result.getSummary() << std::endl;
        return false;
    }
    
    if (result.hasWarnings()) {
        std::cout << "Data validation warnings:\n" << result.getSummary() << std::endl;
    }
    
    return true;
}