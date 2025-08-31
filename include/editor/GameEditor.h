#ifndef GAMEEDITOR_H
#define GAMEEDITOR_H

#include "editor/GameData.h"
#include "ImGuiManager.h"
#include <memory>
#include <string>
#include <optional>

/**
 * Base class for editor panels
 */
class EditorPanel {
public:
    EditorPanel(const std::string& name) : name_(name), visible_(true) {}
    virtual ~EditorPanel() = default;
    
    virtual void render() = 0;
    virtual void update() {}
    
    const std::string& getName() const { return name_; }
    bool isVisible() const { return visible_; }
    void setVisible(bool visible) { visible_ = visible; }

protected:
    std::string name_;
    bool visible_;
};

/**
 * Material editor panel
 */
class MaterialEditorPanel : public EditorPanel {
public:
    MaterialEditorPanel(GameDataManager& dataManager);
    void render() override;

private:
    GameDataManager& dataManager_;
    std::string searchFilter_;
    int selectedMaterialIndex_;
    MaterialTemplate editingMaterial_;
    bool showCreateDialog_;
    
    void renderMaterialList();
    void renderMaterialEditor();
    void renderCreateDialog();
    void renderAttributeEditor(MaterialTemplate& material);
};

/**
 * Recipe editor panel
 */
class RecipeEditorPanel : public EditorPanel {
public:
    RecipeEditorPanel(GameDataManager& dataManager);
    void render() override;

private:
    GameDataManager& dataManager_;
    std::string searchFilter_;
    int selectedRecipeIndex_;
    std::optional<Recipe> editingRecipe_; // Use optional to avoid default constructor requirement
    bool showCreateDialog_;
    
    void renderRecipeList();
    void renderRecipeEditor();
    void renderCreateDialog();
    void renderIngredientEditor(Recipe& recipe);
};

/**
 * Event editor panel
 */
class EventEditorPanel : public EditorPanel {
public:
    EventEditorPanel(GameDataManager& dataManager);
    void render() override;

private:
    GameDataManager& dataManager_;
    std::string searchFilter_;
    int selectedEventIndex_;
    EventTemplate editingEvent_;
    bool showCreateDialog_;
    
    void renderEventList();
    void renderEventEditor();
    void renderCreateDialog();
    void renderConditionEditor(EventTemplate& event);
    void renderEffectEditor(EventTemplate& event);
};

/**
 * Debug tools panel
 */
class DebugPanel : public EditorPanel {
public:
    DebugPanel(GameDataManager& dataManager);
    void render() override;

private:
    GameDataManager& dataManager_;
    bool showValidation_;
    bool showDataInspector_;
    
    void renderValidation();
    void renderDataInspector();
    void renderGameState();
};

/**
 * Main game editor interface
 * Manages all editor panels and provides the main UI
 */
class GameEditor {
public:
    GameEditor();
    ~GameEditor() = default;

    // Initialize editor with ImGui manager
    bool initialize(ImGuiManager& imguiManager);
    
    // Main render function
    void render();
    
    // Update function (called every frame)
    void update();
    
    // Toggle editor visibility
    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }
    
    // Get data manager for external access
    GameDataManager& getDataManager() { return dataManager_; }

private:
    GameDataManager dataManager_;
    ImGuiManager* imguiManager_;
    bool visible_;
    
    // Editor panels
    std::unique_ptr<MaterialEditorPanel> materialPanel_;
    std::unique_ptr<RecipeEditorPanel> recipePanel_;
    std::unique_ptr<EventEditorPanel> eventPanel_;
    std::unique_ptr<DebugPanel> debugPanel_;
    
    // UI state
    int currentTab_;
    bool showDemoWindow_;
    bool showAboutDialog_;
    
    // File operations
    std::string currentFilename_;
    bool hasUnsavedChanges_;
    
    void renderMenuBar();
    void renderTabs();
    void renderStatusBar();
    
    // File operations
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();
    void exportData();
    
    // Edit operations
    void undo();
    void redo();
    
    // View operations
    void showAbout();
    void togglePanel(const std::string& panelName);
    
    // Helper functions
    void markUnsaved() { hasUnsavedChanges_ = true; }
    void markSaved() { hasUnsavedChanges_ = false; }
    std::string getWindowTitle() const;
};

#endif // GAMEEDITOR_H
