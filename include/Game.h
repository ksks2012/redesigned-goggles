#pragma once
#include "game/SimpleGameController.h"
#include "DataManager.h"
#include <memory>

// Forward declarations
class Inventory;
class CraftingSystem;
class Controller;

namespace DataManagement {
    class GameDataManager;
}

/**
 * Refactored Game class
 * Now follows SOLID principles with simplified architecture
 * Acts as a facade for the game system
 * 
 * Architecture:
 * - Follows Facade Pattern - provides simple interface to complex subsystem
 * - Uses SimpleGameController for direct, efficient implementation
 * - Follows Single Responsibility - only handles game lifecycle coordination
 */
class Game {
private:
    std::unique_ptr<GameSystem::SimpleGameController> gameController_;
    
public:
    /**
     * Constructor - creates game with dependency injection
     * Follows Dependency Injection Pattern
     */
    Game();
    
    /**
     * Destructor - cleanup resources
     */
    ~Game() = default;
    
    // Game loop interface
    void run();
    void stop();
    bool isRunning() const;
    void processFrame();
    
    // Game component accessors
    Inventory& getInventory();
    const Inventory& getInventory() const;
    CraftingSystem& getCraftingSystem();
    const CraftingSystem& getCraftingSystem() const;
    Controller& getController();
    const Controller& getController() const;
    DataManagement::GameDataManager& getDataManager();
    
    // Game operations
    bool saveGame();
    bool loadGame();
    bool loadGameData();
    bool saveGameData();
    bool validateGameData();
};