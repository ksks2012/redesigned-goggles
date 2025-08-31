#include "editor/ConsoleEditor.h"
#include "Constants.h"
#include <iostream>
#include <iomanip>
#include <limits>

ConsoleEditor::ConsoleEditor(GameDataManager& dataManager) 
    : dataManager_(dataManager), running_(true) {
}

void ConsoleEditor::run() {
    std::cout << "\n=== GAME EDITOR CONSOLE ===" << std::endl;
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
        } else if (action == "quit" || action == "exit" || action == "q") {
            running_ = false;
        } else {
            std::cout << "Unknown command: " << action << ". Type 'help' for available commands." << std::endl;
        }
    }
}

void ConsoleEditor::showHelp() {
    std::cout << "\n=== AVAILABLE COMMANDS ===" << std::endl;
    std::cout << "material (mat, m) - Material editing commands" << std::endl;
    std::cout << "recipe   (rec, r) - Recipe editing commands" << std::endl;
    std::cout << "event    (evt, e) - Event editing commands" << std::endl;
    std::cout << "data     (d)      - Data operations (save/load/validate)" << std::endl;
    std::cout << "help     (h)      - Show this help" << std::endl;
    std::cout << "quit     (q)      - Exit editor" << std::endl;
}

void ConsoleEditor::handleMaterialCommands() {
    std::cout << "\nMaterial Commands:" << std::endl;
    std::cout << "1. List materials" << std::endl;
    std::cout << "2. Create material" << std::endl;
    std::cout << "3. Edit material" << std::endl;
    std::cout << "4. Delete material" << std::endl;
    std::cout << "5. Show material details" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 5);
    
    switch (choice) {
        case 1: listMaterials(); break;
        case 2: createMaterial(); break;
        case 3: editMaterial(); break;
        case 4: deleteMaterial(); break;
        case 5: {
            std::string id = getUserInput("Enter material ID");
            showMaterial(id);
            break;
        }
        case 0: break;
    }
}

void ConsoleEditor::handleRecipeCommands() {
    std::cout << "\nRecipe Commands:" << std::endl;
    std::cout << "1. List recipes" << std::endl;
    std::cout << "2. Create recipe" << std::endl;
    std::cout << "3. Edit recipe" << std::endl;
    std::cout << "4. Delete recipe" << std::endl;
    std::cout << "5. Show recipe details" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 5);
    
    switch (choice) {
        case 1: listRecipes(); break;
        case 2: createRecipe(); break;
        case 3: editRecipe(); break;
        case 4: deleteRecipe(); break;
        case 5: {
            std::string name = getUserInput("Enter recipe name");
            showRecipe(name);
            break;
        }
        case 0: break;
    }
}

void ConsoleEditor::handleEventCommands() {
    std::cout << "\nEvent Commands:" << std::endl;
    std::cout << "1. List events" << std::endl;
    std::cout << "2. Create event" << std::endl;
    std::cout << "3. Edit event" << std::endl;
    std::cout << "4. Delete event" << std::endl;
    std::cout << "5. Show event details" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 5);
    
    switch (choice) {
        case 1: listEvents(); break;
        case 2: createEvent(); break;
        case 3: editEvent(); break;
        case 4: deleteEvent(); break;
        case 5: {
            std::string id = getUserInput("Enter event ID");
            showEvent(id);
            break;
        }
        case 0: break;
    }
}

void ConsoleEditor::handleDataCommands() {
    std::cout << "\nData Commands:" << std::endl;
    std::cout << "1. Validate data" << std::endl;
    std::cout << "2. Save data" << std::endl;
    std::cout << "3. Load data" << std::endl;
    std::cout << "4. Export data" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
    
    int choice = getUserChoice("Choose option", 0, 4);
    
    switch (choice) {
        case 1: validateData(); break;
        case 2: saveData(); break;
        case 3: loadData(); break;
        case 4: exportData(); break;
        case 0: break;
    }
}

void ConsoleEditor::listMaterials() {
    const auto& materials = dataManager_.getAllMaterials();
    
    std::cout << "\n=== MATERIALS (" << materials.size() << ") ===" << std::endl;
    std::cout << std::left << std::setw(20) << "ID" << std::setw(25) << "Name" 
              << std::setw(15) << "Type" << std::setw(10) << "Rarity" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& material : materials) {
        std::cout << std::left << std::setw(20) << material.id
                  << std::setw(25) << material.name
                  << std::setw(15) << cardTypeToString(material.type)
                  << std::setw(10) << material.rarity << std::endl;
    }
}

void ConsoleEditor::createMaterial() {
    MaterialTemplate material;
    
    std::cout << "\n=== CREATE MATERIAL ===" << std::endl;
    material.id = getUserInput("Material ID");
    
    // Check if ID already exists
    if (dataManager_.getMaterial(material.id)) {
        std::cout << "Error: Material with ID '" << material.id << "' already exists!" << std::endl;
        return;
    }
    
    material.name = getUserInput("Material Name");
    
    // Choose type
    std::cout << "\nCard Types:" << std::endl;
    std::cout << "1. Food    2. Weapon   3. Armor    4. Herb" << std::endl;
    std::cout << "5. Fuel    6. Metal    7. Building 8. Misc" << std::endl;
    int typeChoice = getUserChoice("Choose type", 1, 8);
    
    switch (typeChoice) {
        case 1: material.type = CardType::FOOD; break;
        case 2: material.type = CardType::WEAPON; break;
        case 3: material.type = CardType::ARMOR; break;
        case 4: material.type = CardType::HERB; break;
        case 5: material.type = CardType::FUEL; break;
        case 6: material.type = CardType::METAL; break;
        case 7: material.type = CardType::BUILDING; break;
        case 8: material.type = CardType::MISC; break;
    }
    
    material.rarity = getUserChoice("Rarity (1=Common, 2=Rare, 3=Legendary)", 1, 3);
    material.description = getUserInput("Description (optional)");
    
    // Add attributes
    std::cout << "\nAdd attributes (enter 'done' when finished):" << std::endl;
    while (true) {
        std::string attrName = getUserInput("Attribute name (weight/nutrition/attack/defense/healing/durability/burn_value/crafting_value/trade_value)");
        if (attrName == "done") break;
        
        AttributeType attrType = stringToAttributeType(attrName);
        if (attrType == AttributeType::WEIGHT) { // Using weight as "invalid" indicator
            if (attrName != "weight") {
                std::cout << "Invalid attribute name: " << attrName << std::endl;
                continue;
            }
        }
        
        float value = getUserFloat("Attribute value");
        material.attributes[attrType] = value;
    }
    
    dataManager_.addMaterial(material);
    std::cout << "Material '" << material.name << "' created successfully!" << std::endl;
}

void ConsoleEditor::editMaterial() {
    std::string id = getUserInput("Enter material ID to edit");
    MaterialTemplate* material = dataManager_.getMaterial(id);
    
    if (!material) {
        std::cout << "Material with ID '" << id << "' not found!" << std::endl;
        return;
    }
    
    std::cout << "\n=== EDIT MATERIAL: " << material->name << " ===" << std::endl;
    std::cout << "1. Change name" << std::endl;
    std::cout << "2. Change type" << std::endl;
    std::cout << "3. Change rarity" << std::endl;
    std::cout << "4. Edit attributes" << std::endl;
    std::cout << "5. Change description" << std::endl;
    std::cout << "0. Cancel" << std::endl;
    
    int choice = getUserChoice("Choose what to edit", 0, 5);
    
    switch (choice) {
        case 1:
            material->name = getUserInput("New name");
            break;
        case 2: {
            std::cout << "1. Food  2. Weapon  3. Armor  4. Herb  5. Fuel  6. Metal  7. Building  8. Misc" << std::endl;
            int typeChoice = getUserChoice("Choose new type", 1, 8);
            switch (typeChoice) {
                case 1: material->type = CardType::FOOD; break;
                case 2: material->type = CardType::WEAPON; break;
                case 3: material->type = CardType::ARMOR; break;
                case 4: material->type = CardType::HERB; break;
                case 5: material->type = CardType::FUEL; break;
                case 6: material->type = CardType::METAL; break;
                case 7: material->type = CardType::BUILDING; break;
                case 8: material->type = CardType::MISC; break;
            }
            break;
        }
        case 3:
            material->rarity = getUserChoice("New rarity (1=Common, 2=Rare, 3=Legendary)", 1, 3);
            break;
        case 4: {
            std::cout << "Current attributes:" << std::endl;
            for (const auto& attr : material->attributes) {
                std::cout << "- " << attributeTypeToString(attr.first) << ": " << attr.second << std::endl;
            }
            std::string attrName = getUserInput("Attribute to edit");
            AttributeType attrType = stringToAttributeType(attrName);
            float value = getUserFloat("New value");
            material->attributes[attrType] = value;
            break;
        }
        case 5:
            material->description = getUserInput("New description");
            break;
        case 0:
            return;
    }
    
    dataManager_.updateMaterial(id, *material);
    std::cout << "Material updated successfully!" << std::endl;
}

void ConsoleEditor::deleteMaterial() {
    std::string id = getUserInput("Enter material ID to delete");
    MaterialTemplate* material = dataManager_.getMaterial(id);
    
    if (!material) {
        std::cout << "Material with ID '" << id << "' not found!" << std::endl;
        return;
    }
    
    if (confirmAction("delete material '" + material->name + "'")) {
        dataManager_.removeMaterial(id);
        std::cout << "Material deleted successfully!" << std::endl;
    }
}

void ConsoleEditor::showMaterial(const std::string& id) {
    MaterialTemplate* material = dataManager_.getMaterial(id);
    
    if (!material) {
        std::cout << "Material with ID '" << id << "' not found!" << std::endl;
        return;
    }
    
    std::cout << "\n=== MATERIAL DETAILS ===" << std::endl;
    std::cout << "ID: " << material->id << std::endl;
    std::cout << "Name: " << material->name << std::endl;
    std::cout << "Type: " << cardTypeToString(material->type) << std::endl;
    std::cout << "Rarity: " << material->rarity << std::endl;
    std::cout << "Description: " << material->description << std::endl;
    std::cout << "\nAttributes:" << std::endl;
    
    for (const auto& attr : material->attributes) {
        std::cout << "- " << attributeTypeToString(attr.first) << ": " << attr.second << std::endl;
    }
}

void ConsoleEditor::listRecipes() {
    const auto& recipes = dataManager_.getAllRecipes();
    
    std::cout << "\n=== RECIPES (" << recipes.size() << ") ===" << std::endl;
    for (const auto& recipe : recipes) {
        std::cout << "- " << recipe.name << " (Success: " << (recipe.successRate * 100) << "%)" << std::endl;
    }
}

void ConsoleEditor::createRecipe() {
    std::cout << "\n=== CREATE RECIPE ===" << std::endl;
    std::cout << "Recipe creation not yet fully implemented in console editor." << std::endl;
    std::cout << "This would require selecting materials, quantities, etc." << std::endl;
}

void ConsoleEditor::editRecipe() {
    std::cout << "Recipe editing not yet implemented in console editor." << std::endl;
}

void ConsoleEditor::deleteRecipe() {
    std::cout << "Recipe deletion not yet implemented in console editor." << std::endl;
}

void ConsoleEditor::showRecipe(const std::string& name) {
    std::cout << "Recipe details not yet implemented in console editor." << std::endl;
}

void ConsoleEditor::listEvents() {
    const auto& events = dataManager_.getAllEvents();
    
    std::cout << "\n=== EVENTS (" << events.size() << ") ===" << std::endl;
    for (const auto& event : events) {
        std::cout << "- " << event.id << ": " << event.name << std::endl;
    }
}

void ConsoleEditor::createEvent() {
    std::cout << "Event creation not yet implemented in console editor." << std::endl;
}

void ConsoleEditor::editEvent() {
    std::cout << "Event editing not yet implemented in console editor." << std::endl;
}

void ConsoleEditor::deleteEvent() {
    std::cout << "Event deletion not yet implemented in console editor." << std::endl;
}

void ConsoleEditor::showEvent(const std::string& id) {
    std::cout << "Event details not yet implemented in console editor." << std::endl;
}

void ConsoleEditor::validateData() {
    auto result = dataManager_.validateData();
    
    std::cout << "\n=== DATA VALIDATION ===" << std::endl;
    std::cout << "Status: " << (result.isValid ? "VALID" : "INVALID") << std::endl;
    
    if (!result.errors.empty()) {
        std::cout << "\nErrors:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "- " << error << std::endl;
        }
    }
    
    if (!result.warnings.empty()) {
        std::cout << "\nWarnings:" << std::endl;
        for (const auto& warning : result.warnings) {
            std::cout << "- " << warning << std::endl;
        }
    }
    
    if (result.isValid && result.warnings.empty()) {
        std::cout << "All data is valid!" << std::endl;
    }
}

void ConsoleEditor::saveData() {
    std::string filename = getUserInput("Filename to save to (default: gamedata.json)");
    if (filename.empty()) filename = "gamedata.json";
    
    if (dataManager_.saveToFile(filename)) {
        std::cout << "Data saved to " << filename << std::endl;
    } else {
        std::cout << "Failed to save data to " << filename << std::endl;
    }
}

void ConsoleEditor::loadData() {
    std::string filename = getUserInput("Filename to load from");
    
    if (dataManager_.loadFromFile(filename)) {
        std::cout << "Data loaded from " << filename << std::endl;
    } else {
        std::cout << "Failed to load data from " << filename << std::endl;
    }
}

void ConsoleEditor::exportData() {
    std::cout << "Export functionality not yet implemented." << std::endl;
}

std::string ConsoleEditor::getUserInput(const std::string& prompt) {
    std::cout << prompt << ": ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int ConsoleEditor::getUserChoice(const std::string& prompt, int min, int max) {
    int choice;
    while (true) {
        std::cout << prompt << " (" << min << "-" << max << "): ";
        if (std::cin >> choice && choice >= min && choice <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        } else {
            std::cout << "Invalid input. Please enter a number between " << min << " and " << max << "." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

float ConsoleEditor::getUserFloat(const std::string& prompt) {
    float value;
    while (true) {
        std::cout << prompt << ": ";
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        } else {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

bool ConsoleEditor::confirmAction(const std::string& action) {
    std::string response = getUserInput("Are you sure you want to " + action + "? (yes/no)");
    return response == "yes" || response == "y" || response == "Y";
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
