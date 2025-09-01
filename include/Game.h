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
#include "DataManager.h"
#include <memory>

class Game {
public:
    Game();
    void run();
    
    bool saveGame();
    bool loadGame();
    
    // Data management and version control
    bool loadGameData();
    bool saveGameData() const;
    bool validateGameData() const;
    
    // Provide access to game data for editor
    Inventory& getInventory() { return inventory; }
    const Inventory& getInventory() const { return inventory; }
    CraftingSystem& getCraftingSystem() { return craftingSystem; }
    const CraftingSystem& getCraftingSystem() const { return craftingSystem; }
    Controller& getController() { return controller; }
    const Controller& getController() const { return controller; }
    DataManagement::GameDataManager& getDataManager() { return *globalDataManager; }

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
    
    // Global data management system
    std::unique_ptr<DataManagement::GameDataManager> globalDataManager;
    
    void initializeDefaultGame();
    void initializeEditor();
    void initializeDataSystem();
    void handleEditorInput(SDL_Event& event);
};