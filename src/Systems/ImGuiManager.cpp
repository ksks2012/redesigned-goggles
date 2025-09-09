#include "Systems/ImGuiManager.h"
#include "Systems/DataManager.h"
#include "Interface/editor/ConsoleEditor.h"
#include "Core/Game.h"
#include <iostream>
#include <thread>

// Placeholder implementation without actual ImGui
// This allows the code to compile and run without ImGui installed
// When ImGui is available, this can be replaced with actual implementation

ImGuiManager::ImGuiManager() 
    : initialized_(false), editorMode_(false), io_(nullptr), 
      dataManager_(nullptr), gameInstance_(nullptr), consoleEditor_(nullptr), consoleEditorActive_(false) {
}

ImGuiManager::~ImGuiManager() {
    if (initialized_) {
        shutdown();
    }
}

bool ImGuiManager::initialize(SDL_Window* window, SDL_Renderer* renderer) {
    std::cout << "ImGuiManager: Placeholder initialization (ImGui not available)" << std::endl;
    initialized_ = true;
    return true;
}

void ImGuiManager::beginFrame(SDL_Window* window) {
    if (!initialized_) return;
    // Placeholder - would start ImGui frame here
}

void ImGuiManager::endFrame() {
    if (!initialized_) return;
    // Placeholder - would end ImGui frame here
}

void ImGuiManager::render() {
    if (!initialized_) return;
    // Placeholder - would render ImGui here
}

bool ImGuiManager::handleEvent(SDL_Event* event) {
    if (!initialized_) return false;
    
    // Handle the editor toggle key
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_F1) {
            editorMode_ = !editorMode_;
            std::cout << "Editor mode " << (editorMode_ ? "enabled" : "disabled") << std::endl;
            
            // Notify callback about editor mode change
            if (editorModeCallback_) {
                editorModeCallback_(editorMode_);
            }
            
            // Start console editor when entering editor mode
            if (editorMode_ && !consoleEditorActive_ && dataManager_) {
                startConsoleEditor();
            }
            
            return true;
        }
    }
    
    return false;
}

void ImGuiManager::shutdown() {
    if (!initialized_) return;
    
    std::cout << "ImGuiManager: Shutdown" << std::endl;
    
    // Stop console editor if it's running
    if (consoleEditorActive_ && consoleEditor_) {
        std::cout << "Stopping console editor..." << std::endl;
        consoleEditor_->stop();
        consoleEditorActive_ = false;
    }
    
    initialized_ = false;
}

void ImGuiManager::setDataManager(DataManagement::GameDataManager* dataManager) {
    dataManager_ = dataManager;
}

void ImGuiManager::setGameInstance(Game* game) {
    gameInstance_ = game;
    
    // If console editor is already created, set the game instance
    if (consoleEditor_) {
        consoleEditor_->setGameInstance(game);
    }
}

void ImGuiManager::setupStyle() {
    // Placeholder - would set up ImGui style here
}

void ImGuiManager::setupFonts() {
    // Placeholder - would load fonts here
}

void ImGuiManager::startConsoleEditor() {
    if (!dataManager_) {
        std::cout << "Error: No data manager available for editor" << std::endl;
        return;
    }
    
    if (consoleEditorActive_) {
        std::cout << "Console editor is already running" << std::endl;
        return;
    }
    
    std::cout << "\n=== STARTING CONSOLE EDITOR ===" << std::endl;
    std::cout << "Opening editor in a separate thread..." << std::endl;
    std::cout << "You can use the editor while the game is running!" << std::endl;
    std::cout << "Close the terminal or type 'quit' to exit the editor." << std::endl;
    
    // Start console editor in a separate thread so it doesn't block the game
    std::thread editorThread([this]() {
        consoleEditorActive_ = true;
        
        try {
            if (!consoleEditor_) {
                consoleEditor_ = std::make_unique<ConsoleEditor>(*dataManager_);
                
                // Set game instance if available
                if (gameInstance_) {
                    consoleEditor_->setGameInstance(gameInstance_);
                }
            }
            
            // Run the console editor
            consoleEditor_->run();
            
        } catch (const std::exception& e) {
            std::cout << "Console editor error: " << e.what() << std::endl;
        }
        
        consoleEditorActive_ = false;
        std::cout << "Console editor closed." << std::endl;
    });
    
    // Detach the thread so it runs independently
    editorThread.detach();
}
