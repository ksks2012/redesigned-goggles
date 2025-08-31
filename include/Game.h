#pragma once
#include "SDLManager.h"
#include "Inventory.h"
#include "View.h"
#include "Controller.h"
#include "SaveManager.h"

class Game {
public:
    Game();
    void run();
    
    bool saveGame();
    bool loadGame();

private:
    SDLManager sdlManager;
    Inventory inventory;
    View view;
    Controller controller;
    SaveManager saveManager;
    
    void initializeDefaultGame();
};