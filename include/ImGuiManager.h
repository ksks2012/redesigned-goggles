#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include <SDL2/SDL.h>
#include <memory>
#include <functional>
#include "DataManager.h"

// Forward declarations
struct ImGuiIO;
class ConsoleEditor;
class Game;

/**
 * ImGui integration manager for SDL2
 * Handles ImGui initialization, cleanup, and frame management
 * Note: This requires ImGui to be installed separately
 * Includes console editor fallback when ImGui is not available
 */
class ImGuiManager {
public:
    ImGuiManager();
    ~ImGuiManager();

        // Initialize ImGui manager
    bool initialize(SDL_Window* window, SDL_Renderer* renderer);
    
    // Set data manager for console editor
    void setDataManager(DataManagement::GameDataManager* dataManager);
    
    // Set game instance for console editor syncing
    void setGameInstance(Game* game);
    
    // Frame management
    void beginFrame(SDL_Window* window);
    void endFrame();
    void render();
    
    // Event handling
    bool handleEvent(SDL_Event* event);
    
    // Cleanup
    void shutdown();
    
    // Utility functions
    bool isInitialized() const { return initialized_; }
    
    // Enable/disable editor mode
    void setEditorMode(bool enabled) { editorMode_ = enabled; }
    bool isEditorMode() const { return editorMode_; }
    
    // Editor mode change callback
    using EditorModeCallback = std::function<void(bool)>;
    void setEditorModeCallback(EditorModeCallback callback) { editorModeCallback_ = callback; }

private:
    bool initialized_;
    bool editorMode_;
    ImGuiIO* io_;
    
    // Console editor as fallback
    DataManagement::GameDataManager* dataManager_;
    Game* gameInstance_;
    std::unique_ptr<ConsoleEditor> consoleEditor_;
    bool consoleEditorActive_;
    
    // Editor mode change callback
    EditorModeCallback editorModeCallback_;
    
    void setupStyle();
    void setupFonts();
    void startConsoleEditor();
};

#endif // IMGUIMANAGER_H
