#include "editor/ConsoleEditor.h"
#include "Game.h"

/**
 * Refactored ConsoleEditor implementation
 * Now follows SOLID principles and uses dependency injection
 * Acts as a facade for the new modular editor system
 */

ConsoleEditor::ConsoleEditor(DataManagement::GameDataManager& dataManager) 
    : dataManager_(dataManager), gameInstance_(nullptr) {
    
    // Create the editor controller with dependency injection
    controller_ = Editor::EditorFactory::createConsoleEditor(dataManager, gameInstance_);
}

void ConsoleEditor::setGameInstance(Game* game) {
    gameInstance_ = game;
    
    // Recreate controller with updated game instance
    controller_ = Editor::EditorFactory::createConsoleEditor(dataManager_, gameInstance_);
}

void ConsoleEditor::run() {
    // Delegate to the controller
    controller_->run();
}

void ConsoleEditor::stop() {
    // Delegate to the controller
    controller_->stop();
}
