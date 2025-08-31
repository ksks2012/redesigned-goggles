#pragma once
#include <mutex>
#include <random>
#include <functional>
#include "Inventory.h"
#include "View.h"
#include "Event.h"
#include "CraftingSystem.h"

class Controller {
public:
    Controller(Inventory& inv, View& v, CraftingSystem& crafting);
    void handleEvents();
    bool isRunning() const;
    void updateView();
    void organizeInventory();
    
    void setSaveCallback(std::function<bool()> saveCallback);
    void setLoadCallback(std::function<bool()> loadCallback);

private:
    Inventory& inventory;
    View& view;
    CraftingSystem& craftingSystem;
    Card* selectedCard;
    int mouseX = 0, mouseY = 0;
    bool running;
    bool showCraftingPanel = false;
    std::mutex mutex;
    
    // Save/load callback functions
    std::function<bool()> saveCallback;
    std::function<bool()> loadCallback;

    void handleMouseDown(int x, int y);
    void handleExplore(); // Exploration method
    void handleKeyDown(SDL_Keycode key); // Handle key events
    void handleCrafting(); // Handle crafting panel
    void craftSelectedRecipe(const Recipe& recipe); // Execute crafting
};