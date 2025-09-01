#include "editor/ConsoleEditor.h"
#include "Game.h"
#include "Inventory.h"
#include "CraftingSystem.h"
#include "Controller.h"
#include "Constants.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <set>
#include <map>
#include <algorithm>

ConsoleEditor::ConsoleEditor(DataManagement::GameDataManager& dataManager) 
    : dataManager_(dataManager), gameInstance_(nullptr), running_(true) {
}

void ConsoleEditor::run() {
    std::cout << "\n=== GAME DATA EDITOR CONSOLE ===" << std::endl;
    std::cout << "Connected to new DataManager system with version control" << std::endl;
    std::cout << "Press F1 in-game to toggle editor mode" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl;
    
    while (running_) {
        std::cout << "\nEditor> ";
        std::string command;
        std::getline(std::cin, command);
        
        if (command.empty()) continue;
        
        // Parse command
        std::istringstream iss(command);
        std::string action;
        iss >> action;
        
        if (action == "help" || action == "h") {
            showHelp();
        } else if (action == "material" || action == "mat" || action == "m") {
            handleMaterialCommands();
        } else if (action == "recipe" || action == "rec" || action == "r") {
            handleRecipeCommands();
        } else if (action == "event" || action == "evt" || action == "e") {
            handleEventCommands();
        } else if (action == "data" || action == "d") {
            handleDataCommands();
        } else if (action == "game" || action == "g") {
            handleGameStateCommands();
        } else if (action == "sync") {
            if (gameInstance_) {
                syncFromGame();
            } else {
                std::cout << "No game instance available for sync" << std::endl;
            }
        } else if (action == "apply") {
            if (gameInstance_) {
                syncToGame();
            } else {
                std::cout << "No game instance available for apply" << std::endl;
            }
        } else if (action == "quit" || action == "exit" || action == "q") {
            running_ = false;
        } else {
            std::cout << "Unknown command: " << action << ". Type 'help' for available commands." << std::endl;
        }
    }
}

void ConsoleEditor::showHelp() {
    std::cout << "\n=== EDITOR COMMANDS ===" << std::endl;
    std::cout << "Data Management:" << std::endl;
    std::cout << "  material (mat, m)  - Material operations" << std::endl;
    std::cout << "  recipe (rec, r)    - Recipe operations" << std::endl;
    std::cout << "  event (evt, e)     - Event operations" << std::endl;
    std::cout << "  data (d)           - Data file operations" << std::endl;
    std::cout << "  game (g)           - Game state inspection" << std::endl;
    std::cout << "\nSync Commands:" << std::endl;
    std::cout << "  sync               - Sync data from current game state" << std::endl;
    std::cout << "  apply              - Apply editor changes to game" << std::endl;
    std::cout << "\nGeneral:" << std::endl;
    std::cout << "  help (h)           - Show this help" << std::endl;
    std::cout << "  quit (exit, q)     - Exit editor" << std::endl;
}

void ConsoleEditor::handleMaterialCommands() {
    std::cout << "\n=== MATERIAL OPERATIONS ===" << std::endl;
    std::cout << "1. List materials" << std::endl;
    std::cout << "2. Show material details" << std::endl;
    std::cout << "3. Create material" << std::endl;
    std::cout << "4. Edit material" << std::endl;
    std::cout << "5. Delete material" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 5);
    switch (choice) {
        case 1: listMaterials(); break;
        case 2: {
            std::string name = getUserInput("Material name");
            int rarity = getUserChoice("Material rarity (1-5)", 1, 5);
            showMaterial(name, rarity);
            break;
        }
        case 3: createMaterial(); break;
        case 4: editMaterial(); break;
        case 5: deleteMaterial(); break;
        case 0: break;
    }
}

void ConsoleEditor::handleRecipeCommands() {
    std::cout << "\n=== RECIPE OPERATIONS ===" << std::endl;
    std::cout << "1. List recipes" << std::endl;
    std::cout << "2. Show recipe details" << std::endl;
    std::cout << "3. Create recipe" << std::endl;
    std::cout << "4. Edit recipe" << std::endl;
    std::cout << "5. Delete recipe" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 5);
    switch (choice) {
        case 1: listRecipes(); break;
        case 2: {
            std::string id = getUserInput("Recipe ID");
            showRecipe(id);
            break;
        }
        case 3: createRecipe(); break;
        case 4: editRecipe(); break;
        case 5: deleteRecipe(); break;
        case 0: break;
    }
}

void ConsoleEditor::handleEventCommands() {
    std::cout << "\n=== EVENT OPERATIONS ===" << std::endl;
    std::cout << "1. List events" << std::endl;
    std::cout << "2. Show event details" << std::endl;
    std::cout << "3. Create event" << std::endl;
    std::cout << "4. Edit event" << std::endl;
    std::cout << "5. Delete event" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 5);
    switch (choice) {
        case 1: listEvents(); break;
        case 2: {
            std::string name = getUserInput("Event name");
            showEvent(name);
            break;
        }
        case 3: createEvent(); break;
        case 4: editEvent(); break;
        case 5: deleteEvent(); break;
        case 0: break;
    }
}

void ConsoleEditor::handleDataCommands() {
    std::cout << "\n=== DATA OPERATIONS ===" << std::endl;
    std::cout << "1. Validate data" << std::endl;
    std::cout << "2. Save data" << std::endl;
    std::cout << "3. Load data" << std::endl;
    std::cout << "4. Export data" << std::endl;
    std::cout << "5. Show data status" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 5);
    switch (choice) {
        case 1: validateData(); break;
        case 2: saveData(); break;
        case 3: loadData(); break;
        case 4: exportData(); break;
        case 5: showGameState(); break;
        case 0: break;
    }
}

void ConsoleEditor::handleGameStateCommands() {
    std::cout << "\n=== GAME STATE INSPECTION ===" << std::endl;
    std::cout << "1. Show game state" << std::endl;
    std::cout << "2. Show inventory" << std::endl;
    std::cout << "3. Show recipe status" << std::endl;
    std::cout << "4. Refresh game data" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 4);
    switch (choice) {
        case 1: showGameState(); break;
        case 2: showInventory(); break;
        case 3: showRecipeStatus(); break;
        case 4: refreshGameData(); break;
        case 0: break;
    }
}

void ConsoleEditor::listMaterials() {
    const auto& materials = dataManager_.getMaterials();
    
    std::cout << "\n=== MATERIALS (" << materials.size() << ") ===" << std::endl;
    std::cout << std::left << std::setw(25) << "Name" 
              << std::setw(10) << "Type" << std::setw(8) << "Rarity" 
              << std::setw(10) << "Quantity" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& material : materials) {
        std::cout << std::left << std::setw(25) << material.name
              << std::setw(15) << cardTypeToString(material.type)
              << std::setw(8) << material.rarity 
              << std::setw(10) << material.baseQuantity << std::endl;
    }
}

void ConsoleEditor::createMaterial() {
    DataManagement::MaterialData material;
    
    std::cout << "\n=== CREATE MATERIAL ===" << std::endl;
    material.name = getUserInput("Material Name");
    
    material.rarity = getUserChoice("Rarity (1-5)", 1, 5);
    
    // Check if material already exists
    if (dataManager_.materialExists(material.name, material.rarity)) {
        std::cout << "Error: Material '" << material.name << "' with rarity " 
                  << material.rarity << " already exists!" << std::endl;
        return;
    }
    
    // Choose type
    std::cout << "\nCard Types:" << std::endl;
    std::cout << "0. Metal    1. Weapon   2. Armor    3. Herb" << std::endl;
    std::cout << "4. Food     5. Fuel     6. Building 7. Misc" << std::endl;
    int typeChoice = getUserChoice("Choose type", 0, 7);
    material.type = static_cast<CardType>(typeChoice);
    
    material.baseQuantity = getUserChoice("Base quantity", 1, 100);
    
    // Add to materials
    auto materials = dataManager_.getMaterials();
    materials.push_back(material);
    dataManager_.setMaterials(materials);
    
    std::cout << "Material '" << material.name << "' created successfully!" << std::endl;
}

void ConsoleEditor::editMaterial() {
    std::string name = getUserInput("Material name to edit");
    int rarity = getUserChoice("Material rarity (1-5)", 1, 5);
    
    auto* material = dataManager_.findMaterial(name, rarity);
    if (!material) {
        std::cout << "Material not found: " << name << " (rarity " << rarity << ")" << std::endl;
        return;
    }
    
    std::cout << "\nEditing material: " << material->name << std::endl;
    std::cout << "1. Change name" << std::endl;
    std::cout << "2. Change type" << std::endl;
    std::cout << "3. Change rarity" << std::endl;
    std::cout << "4. Change base quantity" << std::endl;
    std::cout << "0. Cancel" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 4);
    switch (choice) {
        case 1: material->name = getUserInput("New name"); break;
        case 2: {
            std::cout << "0. Metal  1. Weapon  2. Armor  3. Herb" << std::endl;
            std::cout << "4. Food   5. Fuel    6. Building  7. Misc" << std::endl;
            int typeChoice = getUserChoice("Choose type", 0, 7);
            material->type = static_cast<CardType>(typeChoice);
            break;
        }
        case 3: material->rarity = getUserChoice("New rarity (1-5)", 1, 5); break;
        case 4: material->baseQuantity = getUserChoice("New base quantity", 1, 100); break;
        case 0: return;
    }
    
    std::cout << "Material updated successfully!" << std::endl;
}

void ConsoleEditor::deleteMaterial() {
    std::string name = getUserInput("Material name to delete");
    int rarity = getUserChoice("Material rarity (1-5)", 1, 5);
    
    auto materials = dataManager_.getMaterials();
    auto it = std::find_if(materials.begin(), materials.end(), 
                          [&](const DataManagement::MaterialData& m) { 
                              return m.name == name && m.rarity == rarity; 
                          });
    
    if (it == materials.end()) {
        std::cout << "Material not found: " << name << " (rarity " << rarity << ")" << std::endl;
        return;
    }
    
    if (confirmAction("delete material '" + name + "'")) {
        materials.erase(it);
        dataManager_.setMaterials(materials);
        std::cout << "Material '" << name << "' deleted successfully!" << std::endl;
    }
}

void ConsoleEditor::showMaterial(const std::string& name, int rarity) {
    const auto* material = dataManager_.findMaterial(name, rarity);
    if (!material) {
        std::cout << "Material not found: " << name << " (rarity " << rarity << ")" << std::endl;
        return;
    }
    
    std::cout << "\n=== MATERIAL DETAILS ===" << std::endl;
    std::cout << "Name: " << material->name << std::endl;
    std::cout << "Type: " << static_cast<int>(material->type) << std::endl;
    std::cout << "Rarity: " << material->rarity << std::endl;
    std::cout << "Base Quantity: " << material->baseQuantity << std::endl;
    
    if (!material->attributes.empty()) {
        std::cout << "Attributes:" << std::endl;
        for (const auto& attr : material->attributes) {
            std::cout << "  " << static_cast<int>(attr.first) << ": " << attr.second << std::endl;
        }
    }
}

void ConsoleEditor::listRecipes() {
    const auto& recipes = dataManager_.getRecipes();
    
    std::cout << "\n=== RECIPES (" << recipes.size() << ") ===" << std::endl;
    std::cout << std::left << std::setw(15) << "ID" 
              << std::setw(25) << "Name" 
              << std::setw(20) << "Result" 
              << std::setw(10) << "Success %" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& recipe : recipes) {
        std::cout << std::left << std::setw(15) << recipe.id
                  << std::setw(25) << recipe.name
                  << std::setw(20) << recipe.resultMaterial
                  << std::setw(10) << (recipe.successRate * 100) << "%" << std::endl;
    }
}

void ConsoleEditor::createRecipe() {
    std::cout << "\n=== CREATE RECIPE ===" << std::endl;
    std::cout << "This is a simplified interface. For full recipe creation," << std::endl;
    std::cout << "please edit the recipes.json file directly." << std::endl;
}

void ConsoleEditor::editRecipe() {
    std::cout << "Recipe editing not implemented in console editor." << std::endl;
    std::cout << "Please edit recipes.json directly." << std::endl;
}

void ConsoleEditor::deleteRecipe() {
    std::cout << "Recipe deletion not implemented in console editor." << std::endl;
    std::cout << "Please edit recipes.json directly." << std::endl;
}

void ConsoleEditor::showRecipe(const std::string& id) {
    const auto* recipe = dataManager_.findRecipe(id);
    if (!recipe) {
        std::cout << "Recipe not found: " << id << std::endl;
        return;
    }
    
    std::cout << "\n=== RECIPE DETAILS ===" << std::endl;
    std::cout << "ID: " << recipe->id << std::endl;
    std::cout << "Name: " << recipe->name << std::endl;
    std::cout << "Description: " << recipe->description << std::endl;
    std::cout << "Result: " << recipe->resultMaterial << std::endl;
    std::cout << "Success Rate: " << (recipe->successRate * 100) << "%" << std::endl;
    std::cout << "Unlock Level: " << recipe->unlockLevel << std::endl;
    std::cout << "Unlocked: " << (recipe->isUnlocked ? "Yes" : "No") << std::endl;
    
    std::cout << "Ingredients:" << std::endl;
    for (const auto& ingredient : recipe->ingredients) {
        std::cout << "  " << ingredient.first << " x" << ingredient.second << std::endl;
    }
}

void ConsoleEditor::listEvents() {
    const auto& events = dataManager_.getEvents();
    
    std::cout << "\n=== EVENTS (" << events.size() << ") ===" << std::endl;
    std::cout << std::left << std::setw(25) << "Name" 
              << std::setw(40) << "Description" << std::endl;
    std::cout << std::string(65, '-') << std::endl;
    
    for (const auto& event : events) {
        std::string desc = event.description;
        if (desc.length() > 37) {
            desc = desc.substr(0, 34) + "...";
        }
        std::cout << std::left << std::setw(25) << event.name
                  << std::setw(40) << desc << std::endl;
    }
}

void ConsoleEditor::createEvent() {
    std::cout << "\n=== CREATE EVENT ===" << std::endl;
    std::cout << "Event creation not implemented in console editor." << std::endl;
    std::cout << "Please edit events.json directly." << std::endl;
}

void ConsoleEditor::editEvent() {
    std::cout << "Event editing not implemented in console editor." << std::endl;
    std::cout << "Please edit events.json directly." << std::endl;
}

void ConsoleEditor::deleteEvent() {
    std::cout << "Event deletion not implemented in console editor." << std::endl;
    std::cout << "Please edit events.json directly." << std::endl;
}

void ConsoleEditor::showEvent(const std::string& name) {
    const auto& events = dataManager_.getEvents();
    auto it = std::find_if(events.begin(), events.end(),
                          [&](const DataManagement::EventData& e) { return e.name == name; });
    
    if (it == events.end()) {
        std::cout << "Event not found: " << name << std::endl;
        return;
    }
    
    std::cout << "\n=== EVENT DETAILS ===" << std::endl;
    std::cout << "Name: " << it->name << std::endl;
    std::cout << "Description: " << it->description << std::endl;
    
    // Note: EventData structure might need to be checked for actual fields
    std::cout << "This is a placeholder - full event details depend on EventData structure" << std::endl;
}

void ConsoleEditor::validateData() {
    std::cout << "\n=== DATA VALIDATION ===" << std::endl;
    auto result = dataManager_.validateAll();
    
    if (result.isValid) {
        std::cout << "✓ All data is valid!" << std::endl;
    } else {
        std::cout << "✗ Validation failed!" << std::endl;
        if (!result.errors.empty()) {
            std::cout << "Errors:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  ✗ " << error << std::endl;
            }
        }
    }
    
    if (result.hasWarnings()) {
        std::cout << "\nWarnings:" << std::endl;
        for (const auto& warning : result.warnings) {
            std::cout << "  ⚠ " << warning << std::endl;
        }
    }
    
    std::cout << "\n" << result.getSummary() << std::endl;
}

void ConsoleEditor::saveData() {
    std::cout << "\n=== SAVE DATA ===" << std::endl;
    if (dataManager_.saveAllData()) {
        std::cout << "✓ All data saved successfully!" << std::endl;
    } else {
        std::cout << "✗ Failed to save data!" << std::endl;
    }
}

void ConsoleEditor::loadData() {
    std::cout << "\n=== LOAD DATA ===" << std::endl;
    if (dataManager_.loadAllData()) {
        std::cout << "✓ All data loaded successfully!" << std::endl;
    } else {
        std::cout << "✗ Failed to load data!" << std::endl;
    }
}

void ConsoleEditor::exportData() {
    std::string dir = getUserInput("Export directory (default: data_export)");
    if (dir.empty()) dir = "data_export";
    
    std::cout << "\n=== EXPORT DATA ===" << std::endl;
    if (dataManager_.saveAllData(dir + "/")) {
        std::cout << "✓ Data exported to " << dir << "/" << std::endl;
    } else {
        std::cout << "✗ Failed to export data!" << std::endl;
    }
}

void ConsoleEditor::syncFromGame() {
    if (!gameInstance_) {
        std::cout << "No game instance available for sync" << std::endl;
        return;
    }
    
    std::cout << "\n=== SYNC FROM GAME ===" << std::endl;
    std::cout << "Syncing current game state to editor..." << std::endl;
    
    // This is where we would sync from the actual game
    // For now, just load the current data
    if (dataManager_.loadAllData()) {
        std::cout << "✓ Game data synchronized!" << std::endl;
        
        const auto& materials = dataManager_.getMaterials();
        const auto& recipes = dataManager_.getRecipes();
        const auto& events = dataManager_.getEvents();
        
        std::cout << "Loaded:" << std::endl;
        std::cout << "  Materials: " << materials.size() << std::endl;
        std::cout << "  Recipes: " << recipes.size() << std::endl;
        std::cout << "  Events: " << events.size() << std::endl;
    } else {
        std::cout << "✗ Failed to sync game data!" << std::endl;
    }
}

void ConsoleEditor::syncToGame() {
    if (!gameInstance_) {
        std::cout << "No game instance available for sync" << std::endl;
        return;
    }
    
    std::cout << "\n=== SYNC TO GAME ===" << std::endl;
    std::cout << "Applying editor changes to game..." << std::endl;
    
    // Apply data to game systems using the DataManager
    try {
        // Get game systems
        auto& inventory = gameInstance_->getInventory();
        auto& craftingSystem = gameInstance_->getCraftingSystem();
        auto& controller = gameInstance_->getController();
        
        // Apply data
        bool success = true;
        success &= dataManager_.applyToInventory(inventory);
        success &= dataManager_.applyToCraftingSystem(craftingSystem);
        success &= dataManager_.applyToController(controller);
        
        if (success) {
            std::cout << "✓ Changes applied to game successfully!" << std::endl;
        } else {
            std::cout << "⚠ Some changes failed to apply to game" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Error applying changes: " << e.what() << std::endl;
    }
}

void ConsoleEditor::showGameState() {
    std::cout << "\n=== GAME DATA STATUS ===" << std::endl;
    
    const auto& config = dataManager_.getGameConfig();
    const auto& materials = dataManager_.getMaterials();
    const auto& recipes = dataManager_.getRecipes();
    const auto& events = dataManager_.getEvents();
    
    std::cout << "Game Configuration:" << std::endl;
    std::cout << "  Version: " << config.version.toString() << std::endl;
    std::cout << "  Name: " << config.configName << std::endl;
    
    std::cout << "\nData Counts:" << std::endl;
    std::cout << "  Materials: " << materials.size() << std::endl;
    std::cout << "  Recipes: " << recipes.size() << std::endl;
    std::cout << "  Events: " << events.size() << std::endl;
    
    if (gameInstance_) {
        std::cout << "\nGame Instance: Connected" << std::endl;
    } else {
        std::cout << "\nGame Instance: Not connected" << std::endl;
    }
}

void ConsoleEditor::showInventory() {
    if (!gameInstance_) {
        std::cout << "No game instance available" << std::endl;
        return;
    }
    
    std::cout << "\n=== GAME INVENTORY ===" << std::endl;
    const auto& inventory = gameInstance_->getInventory();
    auto cards = inventory.getCards();
    
    std::cout << "Inventory contains " << cards.size() << " items:" << std::endl;
    for (const auto& card : cards) {
        std::cout << "  " << card.name << " x" << card.quantity << std::endl;
    }
}

void ConsoleEditor::showRecipeStatus() {
    if (!gameInstance_) {
        std::cout << "No game instance available" << std::endl;
        return;
    }
    
    std::cout << "\n=== CRAFTING SYSTEM STATUS ===" << std::endl;
    const auto& craftingSystem = gameInstance_->getCraftingSystem();
    auto recipes = craftingSystem.getAllRecipes();
    
    std::cout << "Available recipes:" << std::endl;
    for (const auto& recipe : recipes) {
        std::string status = recipe.isUnlocked ? "Unlocked" : "Locked";
        std::cout << "  " << recipe.name << " - " << status << std::endl;
    }
}

void ConsoleEditor::refreshGameData() {
    std::cout << "\n=== REFRESH GAME DATA ===" << std::endl;
    
    // Reload data from files
    if (dataManager_.loadAllData()) {
        std::cout << "✓ Data refreshed from files" << std::endl;
        
        // If game instance is available, sync the refreshed data
        if (gameInstance_) {
            syncToGame();
        }
    } else {
        std::cout << "✗ Failed to refresh data" << std::endl;
    }
}

// Utility functions
std::string ConsoleEditor::getUserInput(const std::string& prompt) {
    std::cout << prompt << ": ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int ConsoleEditor::getUserChoice(const std::string& prompt, int min, int max) {
    while (true) {
        std::cout << prompt << " (" << min << "-" << max << "): ";
        std::string input;
        std::getline(std::cin, input);
        
        try {
            int choice = std::stoi(input);
            if (choice >= min && choice <= max) {
                return choice;
            }
        } catch (const std::exception&) {
            // Invalid input, continue loop
        }
        
        std::cout << "Invalid input. Please enter a number between " << min << " and " << max << std::endl;
    }
}

float ConsoleEditor::getUserFloat(const std::string& prompt) {
    while (true) {
        std::cout << prompt << ": ";
        std::string input;
        std::getline(std::cin, input);
        
        try {
            return std::stof(input);
        } catch (const std::exception&) {
            std::cout << "Invalid input. Please enter a valid number." << std::endl;
        }
    }
}

bool ConsoleEditor::confirmAction(const std::string& action) {
    std::cout << "Are you sure you want to " << action << "? (y/N): ";
    std::string input;
    std::getline(std::cin, input);
    return input == "y" || input == "Y" || input == "yes" || input == "Yes";
}

std::string ConsoleEditor::attributeTypeToString(AttributeType type) {
    switch (type) {
        case AttributeType::WEIGHT: return "weight";
        case AttributeType::NUTRITION: return "nutrition";
        case AttributeType::ATTACK: return "attack";
        case AttributeType::DEFENSE: return "defense";
        case AttributeType::HEALING: return "healing";
        case AttributeType::DURABILITY: return "durability";
        case AttributeType::BURN_VALUE: return "burn_value";
        case AttributeType::CRAFTING_VALUE: return "crafting_value";
        case AttributeType::TRADE_VALUE: return "trade_value";
        default: return "unknown";
    }
}

AttributeType ConsoleEditor::stringToAttributeType(const std::string& str) {
    if (str == "weight") return AttributeType::WEIGHT;
    if (str == "nutrition") return AttributeType::NUTRITION;
    if (str == "attack") return AttributeType::ATTACK;
    if (str == "defense") return AttributeType::DEFENSE;
    if (str == "healing") return AttributeType::HEALING;
    if (str == "durability") return AttributeType::DURABILITY;
    if (str == "burn_value") return AttributeType::BURN_VALUE;
    if (str == "crafting_value") return AttributeType::CRAFTING_VALUE;
    if (str == "trade_value") return AttributeType::TRADE_VALUE;
    return AttributeType::WEIGHT; // Default/fallback
}

std::string ConsoleEditor::cardTypeToString(CardType type) {
    switch (type) {
        case CardType::FOOD: return "Food";
        case CardType::WEAPON: return "Weapon";
        case CardType::ARMOR: return "Armor";
        case CardType::HERB: return "Herb";
        case CardType::FUEL: return "Fuel";
        case CardType::METAL: return "Metal";
        case CardType::BUILDING: return "Building";
        case CardType::MISC: return "Misc";
        default: return "Unknown";
    }
}

CardType ConsoleEditor::stringToCardType(const std::string& str) {
    if (str == "food" || str == "Food") return CardType::FOOD;
    if (str == "weapon" || str == "Weapon") return CardType::WEAPON;
    if (str == "armor" || str == "Armor") return CardType::ARMOR;
    if (str == "herb" || str == "Herb") return CardType::HERB;
    if (str == "fuel" || str == "Fuel") return CardType::FUEL;
    if (str == "metal" || str == "Metal") return CardType::METAL;
    if (str == "building" || str == "Building") return CardType::BUILDING;
    return CardType::MISC; // Default
}
