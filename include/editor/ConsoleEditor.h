#ifndef CONSOLEEDITOR_H
#define CONSOLEEDITOR_H

#include "GameData.h"
#include <string>
#include <sstream>

/**
 * Console-based editor interface for game data management
 * Provides a command-line interface for editing materials, recipes, and events
 * This serves as a fallback when ImGui is not available
 */
class ConsoleEditor {
public:
    explicit ConsoleEditor(GameDataManager& dataManager);
    
    // Main editor loop
    void run();
    
private:
    GameDataManager& dataManager_;
    bool running_;
    
    // Main command handlers
    void showHelp();
    void handleMaterialCommands();
    void handleRecipeCommands();
    void handleEventCommands();
    void handleDataCommands();
    void handleGameStateCommands(); // New: Game state inspection
    
    // Material operations
    void listMaterials();
    void createMaterial();
    void editMaterial();
    void deleteMaterial();
    void showMaterial(const std::string& id);
    
    // Recipe operations
    void listRecipes();
    void createRecipe();
    void editRecipe();
    void deleteRecipe();
    void showRecipe(const std::string& name);
    
    // Event operations
    void listEvents();
    void createEvent();
    void editEvent();
    void deleteEvent();
    void showEvent(const std::string& id);
    
    // Data operations
    void validateData();
    void saveData();
    void loadData();
    void exportData();
    void syncFromGame();
    void syncToGame();
    
    // Game state inspection
    void showGameState();
    void showInventory();
    void showRecipeStatus();
    void refreshGameData();
    
    // Utility functions
    std::string getUserInput(const std::string& prompt);
    int getUserChoice(const std::string& prompt, int min, int max);
    float getUserFloat(const std::string& prompt);
    bool confirmAction(const std::string& action);
    
    // Type conversion helpers
    std::string attributeTypeToString(AttributeType type);
    AttributeType stringToAttributeType(const std::string& str);
    std::string cardTypeToString(CardType type);
    CardType stringToCardType(const std::string& str);
};

#endif // CONSOLEEDITOR_H
