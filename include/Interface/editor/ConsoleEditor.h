#ifndef CONSOLEEDITOR_H
#define CONSOLEEDITOR_H

#include "EditorController.h"
#include "Systems/DataManager.h"
#include <memory>

// Forward declarations
class Game;

/**
 * Console-based editor interface for game data management
 * Refactored to use SOLID principles and MVC architecture
 * This serves as a facade/adapter for the new editor system
 */
class ConsoleEditor {
public:
    /**
     * Constructor with dependency injection
     * Follows Dependency Injection Pattern
     */
    explicit ConsoleEditor(DataManagement::GameDataManager& dataManager);
    
    /**
     * Set game instance for syncing
     * Follows Interface Segregation Principle (ISP)
     */
    void setGameInstance(Game* game);
    
    /**
     * Main editor loop
     * Delegates to the controller
     */
    void run();
    
    /**
     * Stop the editor
     */
    void stop();

private:
    std::unique_ptr<Editor::EditorController> controller_;
    DataManagement::GameDataManager& dataManager_;
    Game* gameInstance_;
};

#endif // CONSOLEEDITOR_H
