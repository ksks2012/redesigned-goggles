#pragma once
#include <mutex>
#include <random>
#include <functional>
#include <memory>
#include "Inventory.h"
#include "View.h"
#include "Event.h"
#include "CraftingSystem.h"
#include "GameInputHandler.h"

/**
 * Refactored Controller following MVC pattern
 * Delegates input handling to GameInputHandler
 * Focuses on game state coordination and business logic
 * Follows Single Responsibility Principle
 */
class Controller {
public:
    Controller(Inventory& inv, View& v, CraftingSystem& crafting);
    
    // Main game loop operations
    void handleEvents();
    void handleEvent(SDL_Event& event);
    bool isRunning() const;
    void updateView();
    void organizeInventory();
    
    // Game operation callbacks
    void setSaveCallback(std::function<bool()> saveCallback);
    void setLoadCallback(std::function<bool()> loadCallback);
    
    // Editor mode control for organizeInventory
    void pauseOrganizeInventory();
    void resumeOrganizeInventory();

private:
    Inventory& inventory_;
    View& view_;
    CraftingSystem& craftingSystem_;
    
    // Input handling delegation
    std::unique_ptr<GameInputHandler> inputHandler_;
    
    // Game state
    bool organizeInventoryEnabled_ = true;
    std::mutex mutex_;
    
    // Save/load callback functions
    std::function<bool()> saveCallback_;
    std::function<bool()> loadCallback_;

    // Business logic methods
    void handleExplore();
};