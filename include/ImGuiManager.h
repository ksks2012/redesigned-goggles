#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include <SDL2/SDL.h>
#include <memory>
#include <functional>

// Forward declarations
struct ImGuiIO;
class GameDataManager;
class ConsoleEditor;

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

    // Initialize ImGui with SDL2 renderer
    bool initialize(SDL_Window* window, SDL_Renderer* renderer);
    
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
    
    // Console editor integration
    void setDataManager(GameDataManager* dataManager);

private:
    bool initialized_;
    bool editorMode_;
    ImGuiIO* io_;
    
    // Console editor as fallback
    GameDataManager* dataManager_;
    std::unique_ptr<ConsoleEditor> consoleEditor_;
    bool consoleEditorActive_;
    
    void setupStyle();
    void setupFonts();
    void startConsoleEditor();
};

#endif // IMGUIMANAGER_H
