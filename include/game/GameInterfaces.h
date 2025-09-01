#ifndef GAME_INTERFACES_H
#define GAME_INTERFACES_H

#include <memory>
#include <functional>
#include <vector>
#include <string>

// Forward declarations
union SDL_Event; // Correct SDL_Event declaration
class Inventory;
class CraftingSystem;
class Controller;
class Game;

namespace DataManagement {
    class GameDataManager;
}

namespace GameSystem {

/**
 * Interface for rendering operations
 * Follows Interface Segregation Principle (ISP)
 */
class IRenderService {
public:
    virtual ~IRenderService() = default;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void render() = 0;
    virtual void* getWindow() = 0;
    virtual void* getRenderer() = 0;
};

/**
 * Interface for input handling
 * Follows Single Responsibility Principle (SRP)
 */
class IInputService {
public:
    virtual ~IInputService() = default;
    
    virtual bool pollEvents() = 0;
    virtual void handleEvent(SDL_Event& event) = 0;
    virtual bool isRunning() const = 0;
    virtual void stop() = 0;
};

/**
 * Interface for game state persistence
 * Follows Dependency Inversion Principle (DIP)
 */
class IPersistenceService {
public:
    virtual ~IPersistenceService() = default;
    
    virtual bool saveGame() = 0;
    virtual bool loadGame() = 0;
    virtual bool hasExistingSave() const = 0;
    virtual std::string getSaveFilePath() const = 0;
};

/**
 * Interface for data management operations
 * Follows Interface Segregation Principle (ISP)
 */
class IDataService {
public:
    virtual ~IDataService() = default;
    
    virtual bool initializeDataSystem() = 0;
    virtual bool loadGameData() = 0;
    virtual bool saveGameData() = 0;
    virtual bool validateGameData() = 0;
    virtual void applyDataToGameSystems() = 0;
    virtual DataManagement::GameDataManager& getDataManager() = 0;
};

/**
 * Interface for editor system operations
 * Follows Single Responsibility Principle (SRP)
 */
class IEditorService {
public:
    virtual ~IEditorService() = default;
    
    virtual bool initialize(void* window, void* renderer, DataManagement::GameDataManager* dataManager) = 0;
    virtual void shutdown() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual bool handleEvent(SDL_Event* event) = 0;
    virtual bool isEditorMode() const = 0;
    virtual void setEditorModeCallback(std::function<void(bool)> callback) = 0;
    virtual void setGameInstance(Game* game) = 0;
};

/**
 * Interface for game initialization
 * Follows Open/Closed Principle (OCP)
 */
class IGameInitializer {
public:
    virtual ~IGameInitializer() = default;
    
    virtual void initializeDefaultGame() = 0;
    virtual void initializeFromSave() = 0;
    virtual bool setupGameSystems() = 0;
};

/**
 * Interface for game loop management
 * Follows Single Responsibility Principle (SRP)
 */
class IGameLoop {
public:
    virtual ~IGameLoop() = default;
    
    virtual void run() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    virtual void processFrame() = 0;
};

/**
 * Interface for accessing game components
 * Follows Dependency Inversion Principle (DIP)
 */
class IGameComponentsProvider {
public:
    virtual ~IGameComponentsProvider() = default;
    
    virtual Inventory& getInventory() = 0;
    virtual const Inventory& getInventory() const = 0;
    virtual CraftingSystem& getCraftingSystem() = 0;
    virtual const CraftingSystem& getCraftingSystem() const = 0;
    virtual Controller& getController() = 0;
    virtual const Controller& getController() const = 0;
    virtual DataManagement::GameDataManager& getDataManager() = 0;
};

/**
 * Main Game interface
 * Follows Interface Segregation Principle (ISP) - combines focused interfaces
 */
class IGame : public IGameLoop, public IGameComponentsProvider {
public:
    virtual ~IGame() = default;
    
    virtual bool saveGame() = 0;
    virtual bool loadGame() = 0;
    virtual bool loadGameData() = 0;
    virtual bool saveGameData() = 0;
    virtual bool validateGameData() = 0;
};

} // namespace GameSystem

#endif // GAME_INTERFACES_H
