#pragma once
#include <mutex>
#include <random>
#include <functional>
#include "Inventory.h"
#include "View.h"
#include "Event.h"

class Controller {
public:
    Controller(Inventory& inv, View& v);
    void handleEvents();
    bool isRunning() const;
    void updateView();
    void organizeInventory();
    
    void setSaveCallback(std::function<bool()> saveCallback);
    void setLoadCallback(std::function<bool()> loadCallback);

private:
    Inventory& inventory;
    View& view;
    Card* selectedCard;
    int mouseX = 0, mouseY = 0;
    bool running;
    std::mutex mutex;
    
    std::function<bool()> saveCallback;
    std::function<bool()> loadCallback;

    void handleMouseDown(int x, int y);
    void handleExplore(); // New method for exploration
    void handleKeyDown(SDL_Keycode key); // Handle key events
};