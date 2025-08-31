#include "editor/GameEditor.h"
#include <iostream>
#include <algorithm>

// MaterialEditorPanel implementation
MaterialEditorPanel::MaterialEditorPanel(GameDataManager& dataManager)
    : EditorPanel("Material Editor"), dataManager_(dataManager),
      selectedMaterialIndex_(-1), showCreateDialog_(false) {
}

void MaterialEditorPanel::render() {
    // Placeholder implementation without ImGui
    // This will be replaced with actual ImGui code when ImGui is available
    std::cout << "Material Editor Panel - " << dataManager_.getAllMaterials().size() << " materials\n";
}

void MaterialEditorPanel::renderMaterialList() {
    // TODO: Implement with ImGui
}

void MaterialEditorPanel::renderMaterialEditor() {
    // TODO: Implement with ImGui
}

void MaterialEditorPanel::renderCreateDialog() {
    // TODO: Implement with ImGui
}

void MaterialEditorPanel::renderAttributeEditor(MaterialTemplate& material) {
    // TODO: Implement with ImGui
}

// RecipeEditorPanel implementation
RecipeEditorPanel::RecipeEditorPanel(GameDataManager& dataManager)
    : EditorPanel("Recipe Editor"), dataManager_(dataManager),
      selectedRecipeIndex_(-1), showCreateDialog_(false) {
}

void RecipeEditorPanel::render() {
    std::cout << "Recipe Editor Panel - " << dataManager_.getAllRecipes().size() << " recipes\n";
}

void RecipeEditorPanel::renderRecipeList() {
    // TODO: Implement with ImGui
}

void RecipeEditorPanel::renderRecipeEditor() {
    // TODO: Implement with ImGui
}

void RecipeEditorPanel::renderCreateDialog() {
    // TODO: Implement with ImGui
}

void RecipeEditorPanel::renderIngredientEditor(Recipe& recipe) {
    // TODO: Implement with ImGui
}

// EventEditorPanel implementation
EventEditorPanel::EventEditorPanel(GameDataManager& dataManager)
    : EditorPanel("Event Editor"), dataManager_(dataManager),
      selectedEventIndex_(-1), showCreateDialog_(false) {
}

void EventEditorPanel::render() {
    std::cout << "Event Editor Panel - " << dataManager_.getAllEvents().size() << " events\n";
}

void EventEditorPanel::renderEventList() {
    // TODO: Implement with ImGui
}

void EventEditorPanel::renderEventEditor() {
    // TODO: Implement with ImGui
}

void EventEditorPanel::renderCreateDialog() {
    // TODO: Implement with ImGui
}

void EventEditorPanel::renderConditionEditor(EventTemplate& event) {
    // TODO: Implement with ImGui
}

void EventEditorPanel::renderEffectEditor(EventTemplate& event) {
    // TODO: Implement with ImGui
}

// DebugPanel implementation
DebugPanel::DebugPanel(GameDataManager& dataManager)
    : EditorPanel("Debug Tools"), dataManager_(dataManager),
      showValidation_(true), showDataInspector_(false) {
}

void DebugPanel::render() {
    auto validation = dataManager_.validateData();
    std::cout << "Debug Panel - Validation: " << (validation.isValid ? "VALID" : "INVALID") << "\n";
    std::cout << "Errors: " << validation.errors.size() << ", Warnings: " << validation.warnings.size() << "\n";
}

void DebugPanel::renderValidation() {
    // TODO: Implement with ImGui
}

void DebugPanel::renderDataInspector() {
    // TODO: Implement with ImGui
}

void DebugPanel::renderGameState() {
    // TODO: Implement with ImGui
}

// GameEditor implementation
GameEditor::GameEditor()
    : imguiManager_(nullptr), visible_(false), currentTab_(0),
      showDemoWindow_(false), showAboutDialog_(false),
      hasUnsavedChanges_(false) {
    
    // Initialize panels
    materialPanel_ = std::make_unique<MaterialEditorPanel>(dataManager_);
    recipePanel_ = std::make_unique<RecipeEditorPanel>(dataManager_);
    eventPanel_ = std::make_unique<EventEditorPanel>(dataManager_);
    debugPanel_ = std::make_unique<DebugPanel>(dataManager_);
    
    // Set up change callback
    dataManager_.setChangeCallback([this](const std::string& type, const std::string& id) {
        markUnsaved();
        std::cout << "Data changed: " << type << " - " << id << std::endl;
    });
}

bool GameEditor::initialize(ImGuiManager& imguiManager) {
    imguiManager_ = &imguiManager;
    return true;
}

void GameEditor::render() {
    if (!visible_ || !imguiManager_) {
        return;
    }
    
    // For now, just render a simple console output
    std::cout << "=== Game Editor ===" << std::endl;
    std::cout << "Current tab: " << currentTab_ << std::endl;
    
    // Render current panel
    switch (currentTab_) {
        case 0: materialPanel_->render(); break;
        case 1: recipePanel_->render(); break;
        case 2: eventPanel_->render(); break;
        case 3: debugPanel_->render(); break;
    }
    
    std::cout << "===================" << std::endl;
}

void GameEditor::update() {
    // Update panels if needed
    if (visible_) {
        materialPanel_->update();
        recipePanel_->update();
        eventPanel_->update();
        debugPanel_->update();
    }
}

void GameEditor::renderMenuBar() {
    // TODO: Implement with ImGui
    // File menu: New, Open, Save, Save As, Export
    // Edit menu: Undo, Redo
    // View menu: Show/Hide panels
}

void GameEditor::renderTabs() {
    // TODO: Implement with ImGui tabs
}

void GameEditor::renderStatusBar() {
    // TODO: Implement with ImGui
    // Show validation status, file status, etc.
}

void GameEditor::newProject() {
    // Clear current data
    dataManager_ = GameDataManager(); // Reset to defaults
    currentFilename_.clear();
    markSaved();
    std::cout << "New project created" << std::endl;
}

void GameEditor::openProject() {
    // TODO: Implement file dialog
    std::string filename = "gamedata.json"; // Placeholder
    if (dataManager_.loadFromFile(filename)) {
        currentFilename_ = filename;
        markSaved();
        std::cout << "Project opened: " << filename << std::endl;
    } else {
        std::cout << "Failed to open project: " << filename << std::endl;
    }
}

void GameEditor::saveProject() {
    if (currentFilename_.empty()) {
        saveProjectAs();
        return;
    }
    
    if (dataManager_.saveToFile(currentFilename_)) {
        markSaved();
        std::cout << "Project saved: " << currentFilename_ << std::endl;
    } else {
        std::cout << "Failed to save project: " << currentFilename_ << std::endl;
    }
}

void GameEditor::saveProjectAs() {
    // TODO: Implement file dialog
    std::string filename = "gamedata.json"; // Placeholder
    if (dataManager_.saveToFile(filename)) {
        currentFilename_ = filename;
        markSaved();
        std::cout << "Project saved as: " << filename << std::endl;
    } else {
        std::cout << "Failed to save project as: " << filename << std::endl;
    }
}

void GameEditor::exportData() {
    // TODO: Implement export functionality
    std::cout << "Export functionality not yet implemented" << std::endl;
}

void GameEditor::undo() {
    if (dataManager_.undo()) {
        std::cout << "Undo successful" << std::endl;
    } else {
        std::cout << "Nothing to undo" << std::endl;
    }
}

void GameEditor::redo() {
    if (dataManager_.redo()) {
        std::cout << "Redo successful" << std::endl;
    } else {
        std::cout << "Nothing to redo" << std::endl;
    }
}

void GameEditor::showAbout() {
    showAboutDialog_ = true;
}

void GameEditor::togglePanel(const std::string& panelName) {
    if (panelName == "Material") materialPanel_->setVisible(!materialPanel_->isVisible());
    else if (panelName == "Recipe") recipePanel_->setVisible(!recipePanel_->isVisible());
    else if (panelName == "Event") eventPanel_->setVisible(!eventPanel_->isVisible());
    else if (panelName == "Debug") debugPanel_->setVisible(!debugPanel_->isVisible());
}

std::string GameEditor::getWindowTitle() const {
    std::string title = "Game Editor";
    if (!currentFilename_.empty()) {
        title += " - " + currentFilename_;
    }
    if (hasUnsavedChanges_) {
        title += "*";
    }
    return title;
}
