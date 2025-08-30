#pragma once
#include "SDLManager.h"
#include "Inventory.h"
#include "View.h"
#include "Controller.h"

class Game {
public:
    Game();
    void run();

private:
    SDLManager sdlManager;
    Inventory inventory;
    View view;
    Controller controller;
};