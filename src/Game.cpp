#include "Game.h"
#include "Constants.h"
#include <thread>
#include <iostream>

Game::Game() : sdlManager(), inventory(), craftingSystem(), view(sdlManager), controller(inventory, view, craftingSystem), saveManager("game_save.json") {
    // Set save and load callback functions
    controller.setSaveCallback([this]() { return this->saveGame(); });
    controller.setLoadCallback([this]() { return this->loadGame(); });
    
    // Initialize editor system
    initializeEditor();
    
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
        imguiManager.setDataManager(dataManager.get());
        
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