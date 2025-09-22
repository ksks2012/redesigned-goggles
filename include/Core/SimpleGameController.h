#ifndef SIMPLE_GAME_CONTROLLER_H
#define SIMPLE_GAME_CONTROLLER_H

#include "Systems/SDLManager.h"
#include "Core/Inventory.h"
#include "Systems/CraftingSystem.h"
#include "Systems/TechTreeSystem.h"
#include "Core/Controller.h"
#include "Core/View.h"
#include "Core/BaseManager.h"
#include "Systems/SaveManager.h"
#include "Systems/ImGuiManager.h"
#include "Interface/editor/GameEditor.h"
#include "Interface/editor/GameData.h"
#include "Systems/DataManager.h"
#include "Constants.h"
#include <memory>
#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <future>

// Forward declarations
namespace DataManagement {
    class GameDataManager;
}

namespace GameSystem {

/**
 * Simplified Game Controller with better dependency management
 * Follows SOLID principles while avoiding circular dependencies
 * Removed complex interface dependencies for simplicity
 */
class SimpleGameController {
private:
    // Core game components
    std::unique_ptr<SDLManager> sdlManager_;
    std::unique_ptr<Inventory> inventory_;
    std::unique_ptr<CraftingSystem> craftingSystem_;
    std::unique_ptr<TechTreeSystem> techTreeSystem_;
    std::unique_ptr<BaseManager> baseManager_;
    std::unique_ptr<View> view_;
    std::unique_ptr<Controller> controller_;
    std::unique_ptr<SaveManager> saveManager_;
    
    // Editor system
    std::unique_ptr<ImGuiManager> imguiManager_;
    std::unique_ptr<GameEditor> gameEditor_;
    std::unique_ptr<GameDataManager> dataManager_;
    
    // Global data management system
    std::unique_ptr<DataManagement::GameDataManager> globalDataManager_;
    
    // Game state
    bool running_;
    bool shutdown_;
    std::unique_ptr<std::thread> organizerThread_;
    
public:
    /**
     * Constructor with proper initialization order
     * Follows Dependency Injection principles
     */
    SimpleGameController() 
        : sdlManager_(std::make_unique<SDLManager>()),
          inventory_(std::make_unique<Inventory>()),
          craftingSystem_(std::make_unique<CraftingSystem>()),
          baseManager_(std::make_unique<BaseManager>()),
          view_(nullptr), // Will be initialized after SDL
          controller_(nullptr), // Will be initialized after view
          saveManager_(std::make_unique<SaveManager>("game_save.json")),
          imguiManager_(std::make_unique<ImGuiManager>()),
          gameEditor_(nullptr),
          dataManager_(nullptr),
          globalDataManager_(nullptr),
          running_(true), shutdown_(false) {
        
        initialize();
    }
    
    /**
     * Destructor - cleanup resources
     */
    ~SimpleGameController() {
        // Ensure proper shutdown order
        if (running_ && !shutdown_) {
            stop();
            shutdown();
        }
    }
    
    // Game loop interface
    void run() {
        if (!running_) return;
        
        startBackgroundProcesses();
        
        while (running_ && controller_->isRunning()) {
            processFrame();
            
            // Use shorter delay for better responsiveness during shutdown
            if (shutdown_) {
                break; // Immediately exit if shutdown has started
            }
            
            SDL_Delay(Constants::FRAME_DELAY_MS);
        }
        
        shutdown();
    }
    
    void stop() {
        running_ = false;
    }
    
    bool isRunning() const {
        return running_ && (controller_ ? controller_->isRunning() : false);
    }
    
    void processFrame() {
        // Quick exit check for immediate responsiveness
        if (!running_ || shutdown_) {
            return;
        }
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // Handle editor input first
            if (imguiManager_->handleEvent(&event)) {
                continue; // Editor consumed the event
            }
            
            // Handle game input
            controller_->handleEvent(event);
            
            // Check for shutdown after each event for immediate response
            if (!running_ || shutdown_) {
                return;
            }
        }
        
        // Update editor
        if (imguiManager_->isEditorMode() && gameEditor_) {
            imguiManager_->beginFrame(sdlManager_->getWindow());
            gameEditor_->update();
            gameEditor_->render();
            imguiManager_->endFrame();
        }
        
        controller_->updateView();
        
        // Render ImGui overlay
        imguiManager_->render();
    }
    
    // Game component accessors
    Inventory& getInventory() {
        return *inventory_;
    }
    
    const Inventory& getInventory() const {
        return *inventory_;
    }
    
    CraftingSystem& getCraftingSystem() {
        return *craftingSystem_;
    }
    
    const CraftingSystem& getCraftingSystem() const {
        return *craftingSystem_;
    }
    
    TechTreeSystem& getTechTreeSystem() {
        return *techTreeSystem_;
    }
    
    const TechTreeSystem& getTechTreeSystem() const {
        return *techTreeSystem_;
    }
    
    Controller& getController() {
        return *controller_;
    }
    
    const Controller& getController() const {
        return *controller_;
    }
    
    DataManagement::GameDataManager& getDataManager() {
        return *globalDataManager_;
    }
    
    // Game operations
    bool saveGame() {
        return saveManager_->saveGame(*inventory_);
    }
    
    bool loadGame() {
        return saveManager_->loadGame(*inventory_);
    }
    
    bool loadGameData() {
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
        globalDataManager_->applyToInventory(*inventory_);
        globalDataManager_->applyToCraftingSystem(*craftingSystem_);
        globalDataManager_->applyToController(*controller_);
        
        std::cout << "Successfully loaded and applied game data" << std::endl;
        return true;
    }
    
    bool saveGameData() {
        if (!globalDataManager_) {
            std::cerr << "Data manager not initialized" << std::endl;
            return false;
        }
        
        return globalDataManager_->saveAllData();
    }
    
    bool validateGameData() {
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
    
private:
    /**
     * Initialize all game systems in proper order
     * Follows Single Responsibility Principle (SRP)
     */
    bool initialize() {
        // SDL initialization is done in SDLManager constructor
        // Just create the view with SDL manager
        view_ = std::make_unique<View>(*sdlManager_);
        
        // Initialize controller with all dependencies
        controller_ = std::make_unique<Controller>(*inventory_, *view_, *craftingSystem_, *baseManager_);
        
        // Set save and load callback functions
        controller_->setSaveCallback([this]() { return this->saveGame(); });
        controller_->setLoadCallback([this]() { return this->loadGame(); });
        
        // Initialize data management system
        if (!initializeDataSystem()) {
            return false;
        }
        
        // Initialize editor system
        if (!initializeEditor()) {
            return false;
        }
        
        // Load game data from JSON files
        if (!loadGameData()) {
            std::cout << "Game data not found or invalid, creating default data" << std::endl;
            globalDataManager_->createDefaultDataFiles();
            globalDataManager_->saveAllData();
            loadGameData(); // Reload the newly created data
        }
        
        // Try to load the save file; if it fails, initialize the default game
        if (!loadGame()) {
            std::cout << "Save file not found, starting a new game" << std::endl;
            initializeDefaultGame();
        }
        
        return true;
    }
    
    /**
     * Initialize data management system
     * Follows Single Responsibility Principle (SRP)
     */
    bool initializeDataSystem() {
        globalDataManager_ = std::make_unique<DataManagement::GameDataManager>();
        std::cout << "Data management system initialized" << std::endl;
        
        // Initialize TechTreeSystem after DataManager is created
        techTreeSystem_ = std::make_unique<TechTreeSystem>(*sdlManager_, 
                                                           globalDataManager_.get(), 
                                                           craftingSystem_.get());
        std::cout << "Tech tree system initialized" << std::endl;
        
        return true;
    }
    
    /**
     * Initialize editor system
     * Follows Single Responsibility Principle (SRP)
     */
    bool initializeEditor() {
        // Create data manager for editor with game instance reference
        dataManager_ = std::make_unique<GameDataManager>(nullptr); // Avoid circular dependency
        
        if (imguiManager_->initialize(sdlManager_->getWindow(), sdlManager_->getRenderer())) {
            // Provide data manager to ImGuiManager
            imguiManager_->setDataManager(globalDataManager_.get());
            // Note: setGameInstance creates circular dependency, skip for now
            
            // Set callback for editor mode changes to control organizeInventory
            imguiManager_->setEditorModeCallback([this](bool editorMode) {
                if (editorMode) {
                    controller_->pauseOrganizeInventory();
                } else {
                    controller_->resumeOrganizeInventory();
                }
            });
            
            gameEditor_ = std::make_unique<GameEditor>();
            gameEditor_->initialize(*imguiManager_);
            std::cout << "Editor system initialized. Press F1 to toggle editor mode." << std::endl;
            return true;
        } else {
            std::cout << "Failed to initialize editor system" << std::endl;
            return false;
        }
    }
    
    /**
     * Initialize default game state
     * Follows Single Responsibility Principle (SRP)
     */
    void initializeDefaultGame() {
        for (const auto& card : Constants::INITIAL_CARDS) {
            inventory_->addCard(card);
        }
    }
    
    /**
     * Start background processes
     * Follows Single Responsibility Principle (SRP)
     */
    void startBackgroundProcesses() {
        organizerThread_ = std::make_unique<std::thread>([this]() { 
            controller_->organizeInventory(); 
        });
    }
    
    /**
     * Shutdown game systems
     * Follows Single Responsibility Principle (SRP)
     */
    void shutdown() {
        if (shutdown_) return; // Prevent multiple shutdowns
        shutdown_ = true;
        
        std::cout << "Starting graceful shutdown..." << std::endl;
        
        // Stop background processes first with immediate effect
        if (controller_) {
            controller_->stopOrganizeInventory();
        }
        
        // Automatically save when the game ends (with timeout protection)
        std::cout << "Game ended, saving..." << std::endl;
        try {
            auto future = std::async(std::launch::async, [this]() {
                return saveGame();
            });
            
            // Wait for save with shorter timeout
            if (future.wait_for(std::chrono::milliseconds(800)) == std::future_status::timeout) {
                std::cout << "Save operation timed out, continuing shutdown..." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Save error: " << e.what() << ", continuing shutdown..." << std::endl;
        }
        
        // Cleanup background threads
        cleanup();
        
        // Shutdown services
        if (imguiManager_) {
            imguiManager_->shutdown();
        }
        // SDLManager cleanup is handled by destructor
        
        std::cout << "Shutdown complete." << std::endl;
    }
    
    /**
     * Cleanup resources
     * Follows Single Responsibility Principle (SRP)
     */
    void cleanup() {
        if (organizerThread_ && organizerThread_->joinable()) {
            std::cout << "Waiting for background thread to finish..." << std::endl;
            
            // Simple approach: wait for a reasonable time then detach if needed
            bool threadFinished = false;
            std::mutex timeoutMutex;
            std::condition_variable timeoutCv;
            
            std::thread timeoutChecker([&]() {
                if (organizerThread_->joinable()) {
                    organizerThread_->join();
                    std::lock_guard<std::mutex> lock(timeoutMutex);
                    threadFinished = true;
                    timeoutCv.notify_one();
                }
            });
            
            // Wait with timeout - reduce to 300ms for much faster response
            std::unique_lock<std::mutex> lock(timeoutMutex);
            if (timeoutCv.wait_for(lock, std::chrono::milliseconds(300), [&] { return threadFinished; })) {
                std::cout << "Background thread finished normally." << std::endl;
            } else {
                std::cout << "Background thread did not finish in time, detaching..." << std::endl;
                if (organizerThread_->joinable()) {
                    organizerThread_->detach();
                }
            }
            
            if (timeoutChecker.joinable()) {
                timeoutChecker.detach();
            }
            
            organizerThread_.reset();
        }
    }
};

/**
 * Simple Game Factory
 * Follows Factory Pattern
 */
class SimpleGameFactory {
public:
    /**
     * Create a game with proper dependency injection
     * Follows Factory Pattern
     */
    static std::shared_ptr<SimpleGameController> createGame() {
        return std::make_shared<SimpleGameController>();
    }
};

} // namespace GameSystem

#endif // SIMPLE_GAME_CONTROLLER_H
