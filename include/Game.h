#pragma once
#include "game/GameInterfaces.h"
#include "game/SimpleGameController.h"
#include "DataManager.h"
#include <memory>

/**
 * Refactored Game class
 * Now follows SOLID principles and uses dependency injection
 * Acts as a facade for the new modular game system
 * 
 * Architecture:
 * - Follows Facade Pattern - provides simple interface to complex subsystem
 * - Follows Dependency Injection - uses GameController with injected dependencies
 * - Follows Single Responsibility - only handles game lifecycle coordination
 */
class Game : public GameSystem::IGame {
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
    
    // IGameLoop interface implementation
    void run() override;
    void stop() override;
    bool isRunning() const override;
    void processFrame() override;
    
    // IGameComponentsProvider interface implementation
    Inventory& getInventory() override;
    const Inventory& getInventory() const override;
    CraftingSystem& getCraftingSystem() override;
    const CraftingSystem& getCraftingSystem() const override;
    Controller& getController() override;
    const Controller& getController() const override;
    DataManagement::GameDataManager& getDataManager() override;
    
    // Game operations
    bool saveGame() override;
    bool loadGame() override;
    bool loadGameData() override;
    bool saveGameData() override;
    bool validateGameData() override;
};