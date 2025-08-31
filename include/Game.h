#pragma once
#include "SDLManager.h"
#include "Inventory.h"
#include "View.h"
#include "Controller.h"
#include "SaveManager.h"
#include "CraftingSystem.h"
#include "ImGuiManager.h"
#include "editor/GameEditor.h"
#include "editor/GameData.h"
#include <memory>

class Game {
public:
    Game();
    void run();
    
    bool saveGame();
    bool loadGame();
    
    // Provide access to game data for editor
    Inventory& getInventory() { return inventory; }
    const Inventory& getInventory() const { return inventory; }
    CraftingSystem& getCraftingSystem() { return craftingSystem; }
    const CraftingSystem& getCraftingSystem() const { return craftingSystem; }
    Controller& getController() { return controller; }
    const Controller& getController() const { return controller; }

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
    std::unique_ptr<GameDataManager> dataManager;
    
    void initializeDefaultGame();
    void initializeEditor();
    void handleEditorInput(SDL_Event& event);
};