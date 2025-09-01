#ifndef GAME_SERVICES_H
#define GAME_SERVICES_H

#include "game/GameInterfaces.h"
#include "SDLManager.h"
#include "SaveManager.h"
#include "ImGuiManager.h"
#include "editor/GameEditor.h"
#include "DataManager.h"
#include "Constants.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <thread>

namespace GameSystem {

/**
 * SDL-based rendering service implementation
 * Follows Single Responsibility Principle (SRP) - only handles rendering
 */
class SDLRenderService : public IRenderService {
private:
    std::unique_ptr<SDLManager> sdlManager_;
    
public:
    SDLRenderService() : sdlManager_(std::make_unique<SDLManager>()) {}
    
    bool initialize() override {
        return sdlManager_->initialize();
    }
    
    void shutdown() override {
        sdlManager_->cleanup();
    }
    
    void beginFrame() override {
        // SDL rendering preparation if needed
    }
    
    void endFrame() override {
        // SDL rendering finalization if needed
    }
    
    void render() override {
        // Main rendering handled by controller/view
    }
    
    void* getWindow() override {
        return sdlManager_->getWindow();
    }
    
    void* getRenderer() override {
        return sdlManager_->getRenderer();
    }
    
    SDLManager& getSDLManager() { return *sdlManager_; }
    const SDLManager& getSDLManager() const { return *sdlManager_; }
};

/**
 * Game input service implementation
 * Follows Single Responsibility Principle (SRP) - only handles input
 */
class GameInputService : public IInputService {
private:
    bool running_;
    SDL_Event currentEvent_;
    bool hasEvent_;
    
public:
    GameInputService() : running_(true), hasEvent_(false) {}
    
    bool pollEvents() override {
        hasEvent_ = SDL_PollEvent(&currentEvent_);
        return hasEvent_;
    }
    
    void handleEvent(SDL_Event& event) override {
        if (hasEvent_) {
            event = currentEvent_;
        }
    }
    
    bool isRunning() const override {
        return running_;
    }
    
    void stop() override {
        running_ = false;
    }
    
    SDL_Event& getCurrentEvent() { return currentEvent_; }
    bool hasCurrentEvent() const { return hasEvent_; }
};

/**
 * Save/Load persistence service implementation
 * Follows Single Responsibility Principle (SRP) - only handles persistence
 */
class GamePersistenceService : public IPersistenceService {
private:
    SaveManager saveManager_;
    Inventory& inventory_;
    
public:
    explicit GamePersistenceService(Inventory& inventory, const std::string& saveFile = "game_save.json")
        : saveManager_(saveFile), inventory_(inventory) {}
    
    bool saveGame() override {
        return saveManager_.saveGame(inventory_);
    }
    
    bool loadGame() override {
        return saveManager_.loadGame(inventory_);
    }
    
    bool hasExistingSave() const override {
        // Implementation depends on SaveManager's capabilities
        return true; // Placeholder
    }
    
    std::string getSaveFilePath() const override {
        return "game_save.json"; // From saveManager if available
    }
};

/**
 * Data management service implementation
 * Follows Single Responsibility Principle (SRP) - only handles data operations
 */
class GameDataService : public IDataService {
private:
    std::unique_ptr<DataManagement::GameDataManager> globalDataManager_;
    Inventory& inventory_;
    CraftingSystem& craftingSystem_;
    Controller& controller_;
    
public:
    GameDataService(Inventory& inventory, CraftingSystem& craftingSystem, Controller& controller)
        : inventory_(inventory), craftingSystem_(craftingSystem), controller_(controller) {}
    
    bool initializeDataSystem() override {
        globalDataManager_ = std::make_unique<DataManagement::GameDataManager>();
        std::cout << "Data management system initialized" << std::endl;
        return true;
    }
    
    bool loadGameData() override {
        if (!globalDataManager_) {
            std::cerr << "Data manager not initialized" << std::endl;
            return false;
        }
        
        // Try to load all game data
        if (!globalDataManager_->loadAllData()) {
            std::cerr << "Failed to load game data files" << std::endl;
            return false;
        }
        
        // Validate the loaded data
        if (!validateGameData()) {
            std::cerr << "Game data validation failed" << std::endl;
            return false;
        }
        
        // Apply data to game systems
        applyDataToGameSystems();
        
        std::cout << "Successfully loaded and applied game data" << std::endl;
        return true;
    }
    
    bool saveGameData() override {
        if (!globalDataManager_) {
            std::cerr << "Data manager not initialized" << std::endl;
            return false;
        }
        
        return globalDataManager_->saveAllData();
    }
    
    bool validateGameData() override {
        if (!globalDataManager_) {
            std::cerr << "Data manager not initialized" << std::endl;
            return false;
        }
        
        DataManagement::ValidationResult result = globalDataManager_->validateAll();
        
        if (!result.isValid) {
            std::cerr << "Data validation failed:\n" << result.getSummary() << std::endl;
            return false;
        }
        
        if (result.hasWarnings()) {
            std::cout << "Data validation warnings:\n" << result.getSummary() << std::endl;
        }
        
        return true;
    }
    
    void applyDataToGameSystems() override {
        if (globalDataManager_) {
            globalDataManager_->applyToInventory(inventory_);
            globalDataManager_->applyToCraftingSystem(craftingSystem_);
            globalDataManager_->applyToController(controller_);
        }
    }
    
    DataManagement::GameDataManager& getDataManager() override {
        return *globalDataManager_;
    }
    
    bool createDefaultDataIfNeeded() {
        if (!globalDataManager_->loadAllData()) {
            std::cout << "Game data not found or invalid, creating default data" << std::endl;
            globalDataManager_->createDefaultDataFiles();
            globalDataManager_->saveAllData();
            return loadGameData(); // Reload the newly created data
        }
        return true;
    }
};

/**
 * Editor service implementation
 * Follows Single Responsibility Principle (SRP) - only handles editor operations
 */
class GameEditorService : public IEditorService {
private:
    ImGuiManager imguiManager_;
    std::unique_ptr<GameEditor> gameEditor_;
    std::unique_ptr<GameDataManager> dataManager_;
    Game* gameInstance_;
    
public:
    GameEditorService() : gameInstance_(nullptr) {}
    
    bool initialize(void* window, void* renderer, DataManagement::GameDataManager* dataManager) override {
        // Create data manager for editor with game instance reference
        dataManager_ = std::make_unique<GameDataManager>(gameInstance_);
        
        if (imguiManager_.initialize(static_cast<SDL_Window*>(window), static_cast<SDL_Renderer*>(renderer))) {
            // Provide data manager to ImGuiManager
            imguiManager_.setDataManager(dataManager);
            imguiManager_.setGameInstance(gameInstance_);
            
            gameEditor_ = std::make_unique<GameEditor>();
            gameEditor_->initialize(imguiManager_);
            std::cout << "Editor system initialized. Press F1 to toggle editor mode." << std::endl;
            return true;
        } else {
            std::cout << "Failed to initialize editor system" << std::endl;
            return false;
        }
    }
    
    void shutdown() override {
        imguiManager_.shutdown();
    }
    
    void update() override {
        if (gameEditor_ && imguiManager_.isEditorMode()) {
            gameEditor_->update();
        }
    }
    
    void render() override {
        if (gameEditor_ && imguiManager_.isEditorMode()) {
            imguiManager_.beginFrame(static_cast<SDL_Window*>(getWindow()));
            gameEditor_->render();
            imguiManager_.endFrame();
        }
        
        // Render ImGui overlay
        imguiManager_.render();
    }
    
    bool handleEvent(SDL_Event* event) override {
        return imguiManager_.handleEvent(event);
    }
    
    bool isEditorMode() const override {
        return imguiManager_.isEditorMode();
    }
    
    void setEditorModeCallback(std::function<void(bool)> callback) override {
        imguiManager_.setEditorModeCallback(callback);
    }
    
    void setGameInstance(Game* game) override {
        gameInstance_ = game;
        if (dataManager_) {
            dataManager_->setGameInstance(game);
        }
        imguiManager_.setGameInstance(game);
    }
    
private:
    void* getWindow() {
        // This should return the window from render service
        return nullptr; // Placeholder - needs proper integration
    }
};

/**
 * Game initialization service implementation
 * Follows Single Responsibility Principle (SRP) - only handles initialization
 */
class GameInitializationService : public IGameInitializer {
private:
    Inventory& inventory_;
    std::shared_ptr<IPersistenceService> persistenceService_;
    std::shared_ptr<IDataService> dataService_;
    
public:
    GameInitializationService(Inventory& inventory, 
                            std::shared_ptr<IPersistenceService> persistenceService,
                            std::shared_ptr<IDataService> dataService)
        : inventory_(inventory), persistenceService_(persistenceService), dataService_(dataService) {}
    
    void initializeDefaultGame() override {
        for (const auto& card : Constants::INITIAL_CARDS) {
            inventory_.addCard(card);
        }
    }
    
    void initializeFromSave() override {
        if (!persistenceService_->loadGame()) {
            std::cout << "Save file not found, starting a new game" << std::endl;
            initializeDefaultGame();
        }
    }
    
    bool setupGameSystems() override {
        // Initialize data system
        if (!dataService_->initializeDataSystem()) {
            return false;
        }
        
        // Load game data from JSON files
        if (!dataService_->loadGameData()) {
            // Try to create default data if loading fails
            if (auto* gameDataService = dynamic_cast<GameDataService*>(dataService_.get())) {
                return gameDataService->createDefaultDataIfNeeded();
            }
            return false;
        }
        
        return true;
    }
};

} // namespace GameSystem

#endif // GAME_SERVICES_H
