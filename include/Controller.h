#pragma once
#include <mutex>
#include "Inventory.h"
#include "View.h"

class Controller {
public:
    Controller(Inventory& inv, View& v);
    void handleEvents();
    bool isRunning() const;
    void updateView();
    void organizeInventory();

private:
    Inventory& inventory;
    View& view;
    Card* selectedCard;
    int mouseX = 0, mouseY = 0;
    bool running;
    std::mutex mutex;

    void handleMouseDown(int x, int y);
};