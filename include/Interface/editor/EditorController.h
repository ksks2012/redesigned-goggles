#ifndef EDITOR_CONTROLLER_H
#define EDITOR_CONTROLLER_H

#include "EditorInterfaces.h"
#include "EditorServices.h"
#include "EditorCommands.h"
#include <memory>
#include <map>
#include <vector>
#include <algorithm>

namespace Editor {

/**
 * Main Editor Controller
 * Follows MVC Pattern - Controller layer
 * Follows Dependency Inversion Principle (DIP) - depends on abstractions
 * Follows Open/Closed Principle (OCP) - extensible through command registration
 */
class EditorController {
private:
    std::shared_ptr<IUserInterface> ui_;
    std::shared_ptr<IDataService> dataService_;
    std::shared_ptr<IGameStateService> gameStateService_;
    
    std::map<std::string, std::shared_ptr<ICommandHandler>> commands_;
    std::vector<std::string> commandOrder_;
    bool running_;
    
public:
    /**
     * Constructor with dependency injection
     * Follows Dependency Injection Pattern
     */
    EditorController(std::shared_ptr<IUserInterface> ui,
                    std::shared_ptr<IDataService> dataService,
                    std::shared_ptr<IGameStateService> gameStateService)
        : ui_(ui), dataService_(dataService), gameStateService_(gameStateService), running_(true) {
        
        registerDefaultCommands();
    }
    
    /**
     * Register a command handler
     * Follows Open/Closed Principle (OCP) - extensible without modification
     */
    void registerCommand(std::shared_ptr<ICommandHandler> command) {
        if (!command) return;
        
        std::string name = command->getCommandName();
        commands_[name] = command;
        
        // Register aliases
        for (const auto& alias : command->getAliases()) {
            commands_[alias] = command;
        }
        
        // Add to ordered list (avoid duplicates)
        if (std::find(commandOrder_.begin(), commandOrder_.end(), name) == commandOrder_.end()) {
            commandOrder_.push_back(name);
        }
    }
    
    /**
     * Main editor loop
     * Follows Single Responsibility Principle (SRP) - only handles main loop
     */
    void run() {
        ui_->displayMessage("\n=== GAME DATA EDITOR CONSOLE ===");
        ui_->displayMessage("Connected to new DataManager system with version control");
        ui_->displayMessage("Press F1 in-game to toggle editor mode");
        ui_->displayMessage("Type 'help' for available commands");
        
        while (running_) {
            std::string command = ui_->getUserInput("\nEditor");
            if (command.empty()) continue;
            
            executeCommand(command);
        }
    }
    
    /**
     * Execute a command
     * Follows Command Pattern
     */
    void executeCommand(const std::string& commandLine) {
        std::istringstream iss(commandLine);
        std::string action;
        iss >> action;
        
        // Convert to lowercase for case-insensitive matching
        std::transform(action.begin(), action.end(), action.begin(), ::tolower);
        
        if (action == "help" || action == "h") {
            showHelp();
        } else if (action == "quit" || action == "exit" || action == "q") {
            running_ = false;
        } else if (action == "sync") {
            syncFromGame();
        } else if (action == "apply") {
            syncToGame();
        } else {
            auto it = commands_.find(action);
            if (it != commands_.end()) {
                try {
                    it->second->execute();
                } catch (const std::exception& e) {
                    ui_->displayError("Command execution failed: " + std::string(e.what()));
                }
            } else {
                ui_->displayError("Unknown command: " + action + ". Type 'help' for available commands.");
            }
        }
    }
    
    /**
     * Stop the editor
     */
    void stop() {
        running_ = false;
    }
    
private:
    /**
     * Register default commands
     * Follows Single Responsibility Principle (SRP)
     */
    void registerDefaultCommands() {
        // Material command
        auto materialCmd = std::make_shared<MaterialCommandHandler>(ui_, dataService_, gameStateService_);
        registerCommand(materialCmd);
        
        // Recipe command
        auto recipeCmd = std::make_shared<RecipeCommandHandler>(ui_, dataService_, gameStateService_);
        registerCommand(recipeCmd);
        
        // Event command
        auto eventCmd = std::make_shared<EventCommandHandler>(ui_, dataService_, gameStateService_);
        registerCommand(eventCmd);
        
        // Add more commands here as needed...
    }
    
    /**
     * Show help information
     */
    void showHelp() {
        ui_->displayMessage("\n=== EDITOR COMMANDS ===");
        ui_->displayMessage("Data Management:");
        
        for (const auto& cmdName : commandOrder_) {
            auto it = commands_.find(cmdName);
            if (it != commands_.end()) {
                auto cmd = it->second;
                std::string aliases;
                for (const auto& alias : cmd->getAliases()) {
                    if (!aliases.empty()) aliases += ", ";
                    aliases += alias;
                }
                
                std::string line = "  " + cmd->getCommandName();
                if (!aliases.empty()) {
                    line += " (" + aliases + ")";
                }
                line += " - " + cmd->getDescription();
                
                ui_->displayMessage(line);
            }
        }
        
        ui_->displayMessage("\nSync Commands:");
        ui_->displayMessage("  sync               - Sync data from current game state");
        ui_->displayMessage("  apply              - Apply editor changes to game");
        ui_->displayMessage("\nGeneral:");
        ui_->displayMessage("  help (h)           - Show this help");
        ui_->displayMessage("  quit (exit, q)     - Exit editor");
    }
    
    /**
     * Sync data from game
     */
    void syncFromGame() {
        if (!gameStateService_->isGameConnected()) {
            ui_->displayError("No game instance available for sync");
            return;
        }
        
        ui_->displayMessage("\n=== SYNC FROM GAME ===");
        ui_->displayMessage("Syncing current game state to editor...");
        
        if (gameStateService_->syncFromGame()) {
            ui_->displaySuccess("Game data synchronized!");
            
            const auto& materials = dataService_->getMaterials();
            const auto& recipes = dataService_->getRecipes();
            const auto& events = dataService_->getEvents();
            
            ui_->displayMessage("Loaded:");
            ui_->displayMessage("  Materials: " + std::to_string(materials.size()));
            ui_->displayMessage("  Recipes: " + std::to_string(recipes.size()));
            ui_->displayMessage("  Events: " + std::to_string(events.size()));
        } else {
            ui_->displayError("Failed to sync game data!");
        }
    }
    
    /**
     * Sync data to game
     */
    void syncToGame() {
        if (!gameStateService_->isGameConnected()) {
            ui_->displayError("No game instance available for sync");
            return;
        }
        
        ui_->displayMessage("\n=== SYNC TO GAME ===");
        ui_->displayMessage("Applying editor changes to game...");
        
        if (gameStateService_->syncToGame()) {
            ui_->displaySuccess("Changes applied to game successfully!");
        } else {
            ui_->displayWarning("Some changes failed to apply to game");
        }
    }
};

/**
 * Editor Factory - creates and configures the editor
 * Follows Factory Pattern and Dependency Injection
 */
class EditorFactory {
public:
    /**
     * Create a console editor with all dependencies injected
     * Follows Dependency Injection Pattern
     */
    static std::unique_ptr<EditorController> createConsoleEditor(
        DataManagement::GameDataManager& dataManager, 
        Game* gameInstance = nullptr) {
        
        // Create services (dependencies)
        auto ui = std::make_shared<ConsoleUserInterface>();
        auto dataService = std::make_shared<DataManagerService>(dataManager);
        auto gameStateService = std::make_shared<GameStateService>(gameInstance, dataManager);
        
        // Create and configure controller
        auto controller = std::make_unique<EditorController>(ui, dataService, gameStateService);
        
        return controller;
    }
};

} // namespace Editor

#endif // EDITOR_CONTROLLER_H
