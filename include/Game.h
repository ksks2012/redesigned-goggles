#pragma once
#include "SDLManager.h"
#include "Inventory.h"
#include "View.h"
#include "Controller.h"
#include "SaveManager.h"
#include "CraftingSystem.h"
#include "ImGuiManager.h"
#include "editor/GameEditor.h"
#include <memory>

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
    
    // Editor system
    ImGuiManager imguiManager;
    std::unique_ptr<GameEditor> gameEditor;
    
    void initializeDefaultGame();
    void initializeEditor();
    void handleEditorInput(SDL_Event& event);
};