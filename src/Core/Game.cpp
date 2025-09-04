#include "Core/Game.h"
#include "Core/SimpleGameController.h"

/**
 * Refactored Game implementation
 * Now follows SOLID principles and uses dependency injection
 * Acts as a facade for the new modular game system
 */

Game::Game() {
    // Create the game controller with dependency injection
    gameController_ = GameSystem::SimpleGameFactory::createGame();
}

// IGameLoop interface implementation
void Game::run() {
    // Delegate to the controller
    gameController_->run();
}

void Game::stop() {
    // Delegate to the controller
    gameController_->stop();
}

bool Game::isRunning() const {
    // Delegate to the controller
    return gameController_->isRunning();
}

void Game::processFrame() {
    // Delegate to the controller
    gameController_->processFrame();
}

// IGameComponentsProvider interface implementation
Inventory& Game::getInventory() {
    return gameController_->getInventory();
}

const Inventory& Game::getInventory() const {
    return gameController_->getInventory();
}

CraftingSystem& Game::getCraftingSystem() {
    return gameController_->getCraftingSystem();
}

const CraftingSystem& Game::getCraftingSystem() const {
    return gameController_->getCraftingSystem();
}

Controller& Game::getController() {
    return gameController_->getController();
}

const Controller& Game::getController() const {
    return gameController_->getController();
}

DataManagement::GameDataManager& Game::getDataManager() {
    return gameController_->getDataManager();
}

// Game operations
bool Game::saveGame() {
    return gameController_->saveGame();
}

bool Game::loadGame() {
    return gameController_->loadGame();
}

bool Game::loadGameData() {
    return gameController_->loadGameData();
}

bool Game::saveGameData() {
    return gameController_->saveGameData();
}

bool Game::validateGameData() {
    return gameController_->validateGameData();
}