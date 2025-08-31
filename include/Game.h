#pragma once
#include "SDLManager.h"
#include "Inventory.h"
#include "View.h"
#include "Controller.h"
#include "SaveManager.h"
#include "CraftingSystem.h"

class Game {
public:
    Game();
    void run();
    
    bool saveGame();
    bool loadGame();

private:
    SDLManager sdlManager;
    Inventory inventory;
    CraftingSystem craftingSystem;
    View view;
    Controller controller;
    SaveManager saveManager;
    
    void initializeDefaultGame();
};