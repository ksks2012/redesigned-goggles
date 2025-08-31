#include "Game.h"
#include "Constants.h"
#include <thread>
#include <iostream>

Game::Game() : sdlManager(), inventory(), craftingSystem(), view(sdlManager), controller(inventory, view, craftingSystem), saveManager("game_save.json") {
    // Set save and load callback functions
    controller.setSaveCallback([this]() { return this->saveGame(); });
    controller.setLoadCallback([this]() { return this->loadGame(); });
    
    // Try to load the save file; if it fails, initialize the default game
    if (!loadGame()) {
        std::cout << "Save file not found, starting a new game" << std::endl;
        initializeDefaultGame();
    }
}

void Game::run() {
    std::thread organizer([this]() { controller.organizeInventory(); });
    while (controller.isRunning()) {
        controller.handleEvents();
        controller.updateView();
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