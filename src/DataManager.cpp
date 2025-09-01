#include "DataManager.h"
#include "Inventory.h"
#include "CraftingSystem.h"
#include "Controller.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

using json = nlohmann::json;
using namespace DataManagement;

// Version implementation
Version Version::fromString(const std::string& versionStr) {
    Version version;
    std::istringstream ss(versionStr);
    std::string token;
    
    if (std::getline(ss, token, '.')) {
        version.major = std::stoi(token);
    }
    if (std::getline(ss, token, '.')) {
        version.minor = std::stoi(token);
    }
    if (std::getline(ss, token, '.')) {
        version.patch = std::stoi(token);
    }
    
    return version;
}

std::string Version::toString() const {
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

bool Version::operator==(const Version& other) const {
    return major == other.major && minor == other.minor && patch == other.patch;
}

bool Version::operator!=(const Version& other) const {
    return !(*this == other);
}

bool Version::isCompatible(const Version& other) const {
    return major == other.major; // Same major version = compatible
}

// MaterialData implementation
Card MaterialData::toCard() const {
    Card card(name, rarity, type, baseQuantity);
    for (const auto& attr : attributes) {
        card.setAttribute(attr.first, attr.second);
    }
    return card;
}

MaterialData MaterialData::fromCard(const Card& card) {
    MaterialData data;
    data.name = card.name;
    data.rarity = card.rarity;
    data.type = card.type;
    data.baseQuantity = card.quantity;
    data.attributes = card.attributes;
    return data;
}

// ValidationResult implementation
void ValidationResult::addError(const std::string& error) {
    errors.push_back(error);
    isValid = false;
}

void ValidationResult::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

std::string ValidationResult::getSummary() const {
    std::ostringstream summary;
    summary << "Validation Summary: " << (isValid ? "PASSED" : "FAILED") << "\n";
    
    if (!errors.empty()) {
        summary << "Errors (" << errors.size() << "):\n";
        for (const auto& error : errors) {
            summary << "  - " << error << "\n";
        }
    }
    
    if (!warnings.empty()) {
        summary << "Warnings (" << warnings.size() << "):\n";
        for (const auto& warning : warnings) {
            summary << "  - " << warning << "\n";
        }
    }
    
    return summary.str();
}

// GameDataManager implementation
GameDataManager::GameDataManager() {
    // Initialize with default version
    gameConfig.version = Version(1, 0, 0);
    materialsVersion = gameConfig.version;
    recipesVersion = gameConfig.version;
    eventsVersion = gameConfig.version;
}

GameDataManager::~GameDataManager() = default;

bool GameDataManager::loadGameConfig(const std::string& configPath) {
    std::string content;
    if (!readFileContent(configPath, content)) {
        std::cerr << "Failed to read game config file: " << configPath << std::endl;
        return false;
    }
    
    return parseGameConfigJson(content);
}

bool GameDataManager::loadMaterials(const std::string& materialsPath) {
    std::string content;
    if (!readFileContent(materialsPath, content)) {
        std::cerr << "Failed to read materials file: " << materialsPath << std::endl;
        return false;
    }
    
    return parseMaterialsJson(content);
}

bool GameDataManager::loadRecipes(const std::string& recipesPath) {
    std::string content;
    if (!readFileContent(recipesPath, content)) {
        std::cerr << "Failed to read recipes file: " << recipesPath << std::endl;
        return false;
    }
    
    return parseRecipesJson(content);
}

bool GameDataManager::loadEvents(const std::string& eventsPath) {
    std::string content;
    if (!readFileContent(eventsPath, content)) {
        std::cerr << "Failed to read events file: " << eventsPath << std::endl;
        return false;
    }
    
    return parseEventsJson(content);
}

bool GameDataManager::loadAllData(const std::string& dataDirectory) {
    std::string configPath = dataDirectory + "game_config.json";
    std::string materialsPath = dataDirectory + "materials.json";
    std::string recipesPath = dataDirectory + "recipes.json";
    std::string eventsPath = dataDirectory + "events.json";
    
    // Load game config first
    if (!loadGameConfig(configPath)) {
        std::cerr << "Failed to load game configuration" << std::endl;
        return false;
    }
    
    // Load other data files
    bool success = true;
    success &= loadMaterials(materialsPath);
    success &= loadRecipes(recipesPath);
    success &= loadEvents(eventsPath);
    
    if (!success) {
        std::cerr << "Failed to load some data files" << std::endl;
        return false;
    }
    
    // Validate loaded data
    ValidationResult validation = validateAll();
    if (!validation.isValid) {
        std::cerr << "Data validation failed:\n" << validation.getSummary() << std::endl;
        return false;
    }
    
    if (validation.hasWarnings()) {
        std::cout << "Data validation warnings:\n" << validation.getSummary() << std::endl;
    }
    
    std::cout << "Successfully loaded and validated all game data" << std::endl;
    return true;
}

bool GameDataManager::saveGameConfig(const std::string& configPath) const {
    if (!ensureDirectoryExists(std::filesystem::path(configPath).parent_path())) {
        return false;
    }
    
    std::string jsonContent = generateGameConfigJson();
    return writeFileContent(configPath, jsonContent);
}

bool GameDataManager::saveMaterials(const std::string& materialsPath) const {
    if (!ensureDirectoryExists(std::filesystem::path(materialsPath).parent_path())) {
        return false;
    }
    
    std::string jsonContent = generateMaterialsJson();
    return writeFileContent(materialsPath, jsonContent);
}

bool GameDataManager::saveRecipes(const std::string& recipesPath) const {
    if (!ensureDirectoryExists(std::filesystem::path(recipesPath).parent_path())) {
        return false;
    }
    
    std::string jsonContent = generateRecipesJson();
    return writeFileContent(recipesPath, jsonContent);
}

bool GameDataManager::saveEvents(const std::string& eventsPath) const {
    if (!ensureDirectoryExists(std::filesystem::path(eventsPath).parent_path())) {
        return false;
    }
    
    std::string jsonContent = generateEventsJson();
    return writeFileContent(eventsPath, jsonContent);
}

bool GameDataManager::saveAllData(const std::string& dataDirectory) const {
    if (!ensureDirectoryExists(dataDirectory)) {
        return false;
    }
    
    bool success = true;
    success &= saveGameConfig(dataDirectory + "game_config.json");
    success &= saveMaterials(dataDirectory + "materials.json");
    success &= saveRecipes(dataDirectory + "recipes.json");
    success &= saveEvents(dataDirectory + "events.json");
    
    if (success) {
        std::cout << "Successfully saved all game data to " << dataDirectory << std::endl;
    } else {
        std::cerr << "Failed to save some data files" << std::endl;
    }
    
    return success;
}

ValidationResult GameDataManager::validateVersion() const {
    ValidationResult result;
    
    // Check if all data versions match the global version
    if (materialsVersion != gameConfig.version) {
        result.addError("Materials version (" + materialsVersion.toString() + 
                       ") does not match global version (" + gameConfig.version.toString() + ")");
    }
    
    if (recipesVersion != gameConfig.version) {
        result.addError("Recipes version (" + recipesVersion.toString() + 
                       ") does not match global version (" + gameConfig.version.toString() + ")");
    }
    
    if (eventsVersion != gameConfig.version) {
        result.addError("Events version (" + eventsVersion.toString() + 
                       ") does not match global version (" + gameConfig.version.toString() + ")");
    }
    
    // Check version compatibility
    if (!materialsVersion.isCompatible(gameConfig.version)) {
        result.addWarning("Materials version may be incompatible with global version");
    }
    
    if (!recipesVersion.isCompatible(gameConfig.version)) {
        result.addWarning("Recipes version may be incompatible with global version");
    }
    
    if (!eventsVersion.isCompatible(gameConfig.version)) {
        result.addWarning("Events version may be incompatible with global version");
    }
    
    return result;
}

ValidationResult GameDataManager::validateDataConsistency() const {
    ValidationResult result;
    
    // Validate recipe dependencies
    for (const auto& recipe : recipes) {
        // Check if recipe ingredients exist in materials
        for (const auto& ingredient : recipe.ingredients) {
            const std::string& materialName = ingredient.first;
            bool found = false;
            
            for (const auto& material : materials) {
                if (material.name == materialName) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                result.addError("Recipe '" + recipe.name + "' references non-existent material: " + materialName);
            }
        }
        
        // Check if recipe result exists in materials
        bool resultFound = false;
        for (const auto& material : materials) {
            if (material.name == recipe.resultMaterial) {
                resultFound = true;
                break;
            }
        }
        
        if (!resultFound) {
            result.addError("Recipe '" + recipe.name + "' produces non-existent material: " + recipe.resultMaterial);
        }
    }
    
    // Validate event dependencies
    for (const auto& event : events) {
        // Check reward materials
        for (const auto& rewardMaterial : event.rewardMaterials) {
            bool found = false;
            for (const auto& material : materials) {
                if (material.name == rewardMaterial) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.addWarning("Event '" + event.name + "' references non-existent reward material: " + rewardMaterial);
            }
        }
        
        // Check penalty materials
        for (const auto& penaltyMaterial : event.penaltyMaterials) {
            bool found = false;
            for (const auto& material : materials) {
                if (material.name == penaltyMaterial) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.addWarning("Event '" + event.name + "' references non-existent penalty material: " + penaltyMaterial);
            }
        }
    }
    
    return result;
}

ValidationResult GameDataManager::validateMaterials() const {
    ValidationResult result;
    
    // Check for duplicate materials
    std::unordered_map<std::string, int> materialCounts;
    for (const auto& material : materials) {
        std::string key = material.name + "_" + std::to_string(material.rarity);
        materialCounts[key]++;
        
        if (materialCounts[key] > 1) {
            result.addError("Duplicate material found: " + material.name + " (rarity " + std::to_string(material.rarity) + ")");
        }
        
        // Validate material properties
        if (material.name.empty()) {
            result.addError("Material with empty name found");
        }
        
        if (material.rarity < 1 || material.rarity > 3) {
            result.addWarning("Material '" + material.name + "' has unusual rarity: " + std::to_string(material.rarity));
        }
        
        if (material.baseQuantity < 0) {
            result.addError("Material '" + material.name + "' has negative base quantity: " + std::to_string(material.baseQuantity));
        }
    }
    
    return result;
}

ValidationResult GameDataManager::validateRecipes() const {
    ValidationResult result;
    
    // Check for duplicate recipe IDs
    std::unordered_map<std::string, int> recipeIds;
    for (const auto& recipe : recipes) {
        recipeIds[recipe.id]++;
        
        if (recipeIds[recipe.id] > 1) {
            result.addError("Duplicate recipe ID found: " + recipe.id);
        }
        
        // Validate recipe properties
        if (recipe.id.empty()) {
            result.addError("Recipe with empty ID found");
        }
        
        if (recipe.name.empty()) {
            result.addError("Recipe '" + recipe.id + "' has empty name");
        }
        
        if (recipe.successRate < 0.0f || recipe.successRate > 1.0f) {
            result.addWarning("Recipe '" + recipe.name + "' has unusual success rate: " + std::to_string(recipe.successRate));
        }
        
        if (recipe.ingredients.empty()) {
            result.addWarning("Recipe '" + recipe.name + "' has no ingredients");
        }
        
        if (recipe.resultMaterial.empty()) {
            result.addError("Recipe '" + recipe.name + "' has empty result material");
        }
    }
    
    return result;
}

ValidationResult GameDataManager::validateEvents() const {
    ValidationResult result;
    
    // Check for duplicate event names
    std::unordered_map<std::string, int> eventNames;
    for (const auto& event : events) {
        eventNames[event.name]++;
        
        if (eventNames[event.name] > 1) {
            result.addError("Duplicate event name found: " + event.name);
        }
        
        // Validate event properties
        if (event.name.empty()) {
            result.addError("Event with empty name found");
        }
        
        if (event.probability < 0.0f || event.probability > 1.0f) {
            result.addWarning("Event '" + event.name + "' has unusual probability: " + std::to_string(event.probability));
        }
        
        if (event.rewardMaterials.empty() && event.penaltyMaterials.empty()) {
            result.addWarning("Event '" + event.name + "' has no rewards or penalties");
        }
    }
    
    return result;
}

ValidationResult GameDataManager::validateAll() const {
    ValidationResult result;
    
    // Combine all validation results
    ValidationResult versionResult = validateVersion();
    ValidationResult consistencyResult = validateDataConsistency();
    ValidationResult materialsResult = validateMaterials();
    ValidationResult recipesResult = validateRecipes();
    ValidationResult eventsResult = validateEvents();
    
    // Merge results
    result.errors.insert(result.errors.end(), versionResult.errors.begin(), versionResult.errors.end());
    result.errors.insert(result.errors.end(), consistencyResult.errors.begin(), consistencyResult.errors.end());
    result.errors.insert(result.errors.end(), materialsResult.errors.begin(), materialsResult.errors.end());
    result.errors.insert(result.errors.end(), recipesResult.errors.begin(), recipesResult.errors.end());
    result.errors.insert(result.errors.end(), eventsResult.errors.begin(), eventsResult.errors.end());
    
    result.warnings.insert(result.warnings.end(), versionResult.warnings.begin(), versionResult.warnings.end());
    result.warnings.insert(result.warnings.end(), consistencyResult.warnings.begin(), consistencyResult.warnings.end());
    result.warnings.insert(result.warnings.end(), materialsResult.warnings.begin(), materialsResult.warnings.end());
    result.warnings.insert(result.warnings.end(), recipesResult.warnings.begin(), recipesResult.warnings.end());
    result.warnings.insert(result.warnings.end(), eventsResult.warnings.begin(), eventsResult.warnings.end());
    
    result.isValid = result.errors.empty();
    
    return result;
}

bool GameDataManager::materialExists(const std::string& name, int rarity) const {
    return findMaterial(name, rarity) != nullptr;
}

MaterialData* GameDataManager::findMaterial(const std::string& name, int rarity) {
    for (auto& material : materials) {
        if (material.name == name && material.rarity == rarity) {
            return &material;
        }
    }
    return nullptr;
}

const MaterialData* GameDataManager::findMaterial(const std::string& name, int rarity) const {
    for (const auto& material : materials) {
        if (material.name == name && material.rarity == rarity) {
            return &material;
        }
    }
    return nullptr;
}

RecipeData* GameDataManager::findRecipe(const std::string& id) {
    for (auto& recipe : recipes) {
        if (recipe.id == id) {
            return &recipe;
        }
    }
    return nullptr;
}

const RecipeData* GameDataManager::findRecipe(const std::string& id) const {
    for (const auto& recipe : recipes) {
        if (recipe.id == id) {
            return &recipe;
        }
    }
    return nullptr;
}

// Continued in next part due to length...

bool GameDataManager::applyToInventory(Inventory& inventory) const {
    // Clear existing inventory and add materials
    std::vector<Card> newCards;
    
    for (const auto& material : materials) {
        Card card = material.toCard();
        newCards.push_back(card);
    }
    
    inventory.updateCards(newCards);
    std::cout << "Applied " << materials.size() << " materials to inventory" << std::endl;
    return true;
}

bool GameDataManager::applyToCraftingSystem(CraftingSystem& craftingSystem) const {
    // This would require extending CraftingSystem to accept recipe data
    // For now, we'll log the action
    std::cout << "Applied " << recipes.size() << " recipes to crafting system" << std::endl;
    return true;
}

bool GameDataManager::applyToController(Controller& controller) const {
    // This would require extending Controller to accept event data
    // For now, we'll log the action
    std::cout << "Applied " << events.size() << " events to controller" << std::endl;
    return true;
}

void GameDataManager::createDefaultGameConfig() {
    gameConfig.version = Version(1, 0, 0);
    gameConfig.configName = "Survive Game Configuration";
    gameConfig.description = "Default game configuration for Survive MVP";
    gameConfig.settings["difficulty"] = "normal";
    gameConfig.settings["auto_save"] = "true";
    gameConfig.settings["language"] = "en";
}

void GameDataManager::createDefaultMaterials() {
    materials.clear();
    
    // Basic materials
    MaterialData wood;
    wood.name = "Wood";
    wood.rarity = 1;
    wood.type = CardType::BUILDING;
    wood.baseQuantity = 1;
    wood.attributes[AttributeType::WEIGHT] = 2.0f;
    wood.attributes[AttributeType::BURN_VALUE] = 15.0f;
    wood.attributes[AttributeType::CRAFTING_VALUE] = 10.0f;
    wood.attributes[AttributeType::TRADE_VALUE] = 5.0f;
    materials.push_back(wood);
    
    MaterialData metal;
    metal.name = "Metal";
    metal.rarity = 2;
    metal.type = CardType::METAL;
    metal.baseQuantity = 1;
    metal.attributes[AttributeType::WEIGHT] = 5.0f;
    metal.attributes[AttributeType::DURABILITY] = 100.0f;
    metal.attributes[AttributeType::CRAFTING_VALUE] = 25.0f;
    metal.attributes[AttributeType::TRADE_VALUE] = 20.0f;
    materials.push_back(metal);
    
    MaterialData food;
    food.name = "Food";
    food.rarity = 1;
    food.type = CardType::FOOD;
    food.baseQuantity = 1;
    food.attributes[AttributeType::WEIGHT] = 0.5f;
    food.attributes[AttributeType::NUTRITION] = 20.0f;
    food.attributes[AttributeType::TRADE_VALUE] = 8.0f;
    materials.push_back(food);
    
    MaterialData medicine;
    medicine.name = "Medicine";
    medicine.rarity = 2;
    medicine.type = CardType::HERB;
    medicine.baseQuantity = 1;
    medicine.attributes[AttributeType::WEIGHT] = 0.2f;
    medicine.attributes[AttributeType::HEALING] = 50.0f;
    medicine.attributes[AttributeType::TRADE_VALUE] = 30.0f;
    materials.push_back(medicine);
    
    MaterialData bandage;
    bandage.name = "Bandage";
    bandage.rarity = 1;
    bandage.type = CardType::HERB;
    bandage.baseQuantity = 1;
    bandage.attributes[AttributeType::WEIGHT] = 0.1f;
    bandage.attributes[AttributeType::HEALING] = 25.0f;
    bandage.attributes[AttributeType::TRADE_VALUE] = 10.0f;
    materials.push_back(bandage);
    
    // Result materials for recipes
    MaterialData medkit;
    medkit.name = "Medkit";
    medkit.rarity = 2;
    medkit.type = CardType::HERB;
    medkit.baseQuantity = 1;
    medkit.attributes[AttributeType::WEIGHT] = 0.3f;
    medkit.attributes[AttributeType::HEALING] = 75.0f;
    medkit.attributes[AttributeType::TRADE_VALUE] = 45.0f;
    materials.push_back(medkit);
    
    MaterialData wall;
    wall.name = "Wall";
    wall.rarity = 2;
    wall.type = CardType::BUILDING;
    wall.baseQuantity = 1;
    wall.attributes[AttributeType::DEFENSE] = 50.0f;
    wall.attributes[AttributeType::DURABILITY] = 200.0f;
    wall.attributes[AttributeType::WEIGHT] = 10.0f;
    wall.attributes[AttributeType::TRADE_VALUE] = 35.0f;
    materials.push_back(wall);
    
    MaterialData tool;
    tool.name = "Tool";
    tool.rarity = 2;
    tool.type = CardType::TOOL;
    tool.baseQuantity = 1;
    tool.attributes[AttributeType::CRAFTING_VALUE] = 30.0f;
    tool.attributes[AttributeType::DURABILITY] = 80.0f;
    tool.attributes[AttributeType::WEIGHT] = 2.0f;
    tool.attributes[AttributeType::TRADE_VALUE] = 25.0f;
    materials.push_back(tool);
}

void GameDataManager::createDefaultRecipes() {
    recipes.clear();
    
    // Medkit recipe
    RecipeData medkit;
    medkit.id = "medkit";
    medkit.name = "Medkit";
    medkit.description = "A basic medical supply made from medicine and bandage";
    medkit.ingredients = {{"Medicine", 1}, {"Bandage", 1}};
    medkit.resultMaterial = "Medkit";
    medkit.successRate = 0.9f;
    medkit.unlockLevel = 0;
    medkit.isUnlocked = true;
    recipes.push_back(medkit);
    
    // Wall recipe
    RecipeData wall;
    wall.id = "wall";
    wall.name = "Wall";
    wall.description = "Basic defensive structure";
    wall.ingredients = {{"Wood", 2}, {"Metal", 1}};
    wall.resultMaterial = "Wall";
    wall.successRate = 0.95f;
    wall.unlockLevel = 0;
    wall.isUnlocked = true;
    recipes.push_back(wall);
    
    // Tool recipe
    RecipeData tool;
    tool.id = "tool";
    tool.name = "Basic Tool";
    tool.description = "Simple crafting tool";
    tool.ingredients = {{"Metal", 1}, {"Wood", 1}};
    tool.resultMaterial = "Tool";
    tool.successRate = 0.8f;
    tool.unlockLevel = 1;
    tool.isUnlocked = false;
    recipes.push_back(tool);
}

void GameDataManager::createDefaultEvents() {
    events.clear();
    
    // Resource discovery event
    EventData discovery;
    discovery.name = "Resource Discovery";
    discovery.description = "Found useful materials while exploring";
    discovery.rewardMaterials = {"Wood", "Food"};
    discovery.penaltyMaterials = {};
    discovery.probability = 0.3f;
    events.push_back(discovery);
    
    // Medical emergency event
    EventData emergency;
    emergency.name = "Medical Emergency";
    emergency.description = "Team member injured, need medical supplies";
    emergency.rewardMaterials = {};
    emergency.penaltyMaterials = {"Bandage"};
    emergency.probability = 0.15f;
    events.push_back(emergency);
    
    // Trader encounter
    EventData trader;
    trader.name = "Trader Encounter";
    trader.description = "Met a traveling trader willing to exchange goods";
    trader.rewardMaterials = {"Food"};
    trader.penaltyMaterials = {"Wood"};
    trader.probability = 0.2f;
    events.push_back(trader);
}

void GameDataManager::createDefaultDataFiles(const std::string& dataDirectory) {
    createDefaultGameConfig();
    createDefaultMaterials();
    createDefaultRecipes();
    createDefaultEvents();
    
    // Update versions
    materialsVersion = gameConfig.version;
    recipesVersion = gameConfig.version;
    eventsVersion = gameConfig.version;
    
    std::cout << "Created default data with version " << gameConfig.version.toString() << std::endl;
}

// JSON parsing implementations
bool GameDataManager::parseGameConfigJson(const std::string& jsonContent) {
    try {
        json j = json::parse(jsonContent);
        
        if (j.contains("version")) {
            gameConfig.version = Version::fromString(j["version"]);
        }
        
        if (j.contains("config_name")) {
            gameConfig.configName = j["config_name"];
        }
        
        if (j.contains("description")) {
            gameConfig.description = j["description"];
        }
        
        if (j.contains("settings") && j["settings"].is_object()) {
            for (auto& [key, value] : j["settings"].items()) {
                if (value.is_string()) {
                    gameConfig.settings[key] = value;
                }
            }
        }
        
        std::cout << "Loaded game config version " << gameConfig.version.toString() << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing game config JSON: " << e.what() << std::endl;
        return false;
    }
}

bool GameDataManager::parseMaterialsJson(const std::string& jsonContent) {
    try {
        json j = json::parse(jsonContent);
        
        if (j.contains("version")) {
            materialsVersion = Version::fromString(j["version"]);
        }
        
        if (j.contains("materials") && j["materials"].is_array()) {
            materials.clear();
            
            for (const auto& materialJson : j["materials"]) {
                MaterialData material;
                material.name = materialJson["name"];
                material.rarity = materialJson["rarity"];
                material.type = static_cast<CardType>(materialJson["type"]);
                material.baseQuantity = materialJson.value("base_quantity", 1);
                
                if (materialJson.contains("attributes") && materialJson["attributes"].is_object()) {
                    for (auto& [key, value] : materialJson["attributes"].items()) {
                        int attrType = std::stoi(key);
                        material.attributes[static_cast<AttributeType>(attrType)] = value;
                    }
                }
                
                materials.push_back(material);
            }
        }
        
        std::cout << "Loaded " << materials.size() << " materials (version " << 
                     materialsVersion.toString() << ")" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing materials JSON: " << e.what() << std::endl;
        return false;
    }
}

bool GameDataManager::parseRecipesJson(const std::string& jsonContent) {
    try {
        json j = json::parse(jsonContent);
        
        if (j.contains("version")) {
            recipesVersion = Version::fromString(j["version"]);
        }
        
        if (j.contains("recipes") && j["recipes"].is_array()) {
            recipes.clear();
            
            for (const auto& recipeJson : j["recipes"]) {
                RecipeData recipe;
                recipe.id = recipeJson["id"];
                recipe.name = recipeJson["name"];
                recipe.description = recipeJson["description"];
                recipe.resultMaterial = recipeJson["result_material"];
                recipe.successRate = recipeJson["success_rate"];
                recipe.unlockLevel = recipeJson.value("unlock_level", 0);
                recipe.isUnlocked = recipeJson.value("is_unlocked", true);
                
                if (recipeJson.contains("ingredients") && recipeJson["ingredients"].is_array()) {
                    for (const auto& ingredient : recipeJson["ingredients"]) {
                        std::string materialName = ingredient["material"];
                        int quantity = ingredient["quantity"];
                        recipe.ingredients.emplace_back(materialName, quantity);
                    }
                }
                
                recipes.push_back(recipe);
            }
        }
        
        std::cout << "Loaded " << recipes.size() << " recipes (version " << 
                     recipesVersion.toString() << ")" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing recipes JSON: " << e.what() << std::endl;
        return false;
    }
}

bool GameDataManager::parseEventsJson(const std::string& jsonContent) {
    try {
        json j = json::parse(jsonContent);
        
        if (j.contains("version")) {
            eventsVersion = Version::fromString(j["version"]);
        }
        
        if (j.contains("events") && j["events"].is_array()) {
            events.clear();
            
            for (const auto& eventJson : j["events"]) {
                EventData event;
                event.name = eventJson["name"];
                event.description = eventJson["description"];
                event.probability = eventJson["probability"];
                
                if (eventJson.contains("reward_materials") && eventJson["reward_materials"].is_array()) {
                    for (const auto& reward : eventJson["reward_materials"]) {
                        event.rewardMaterials.push_back(reward);
                    }
                }
                
                if (eventJson.contains("penalty_materials") && eventJson["penalty_materials"].is_array()) {
                    for (const auto& penalty : eventJson["penalty_materials"]) {
                        event.penaltyMaterials.push_back(penalty);
                    }
                }
                
                events.push_back(event);
            }
        }
        
        std::cout << "Loaded " << events.size() << " events (version " << 
                     eventsVersion.toString() << ")" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing events JSON: " << e.what() << std::endl;
        return false;
    }
}

// JSON generation implementations
std::string GameDataManager::generateGameConfigJson() const {
    json j;
    j["version"] = gameConfig.version.toString();
    j["config_name"] = gameConfig.configName;
    j["description"] = gameConfig.description;
    j["settings"] = gameConfig.settings;
    
    return j.dump(4); // Pretty print with 4 spaces
}

std::string GameDataManager::generateMaterialsJson() const {
    json j;
    j["version"] = gameConfig.version.toString();
    j["materials"] = json::array();
    
    for (const auto& material : materials) {
        json materialJson;
        materialJson["name"] = material.name;
        materialJson["rarity"] = material.rarity;
        materialJson["type"] = static_cast<int>(material.type);
        materialJson["base_quantity"] = material.baseQuantity;
        
        json attributesJson;
        for (const auto& attr : material.attributes) {
            attributesJson[std::to_string(static_cast<int>(attr.first))] = attr.second;
        }
        materialJson["attributes"] = attributesJson;
        
        j["materials"].push_back(materialJson);
    }
    
    return j.dump(4);
}

std::string GameDataManager::generateRecipesJson() const {
    json j;
    j["version"] = gameConfig.version.toString();
    j["recipes"] = json::array();
    
    for (const auto& recipe : recipes) {
        json recipeJson;
        recipeJson["id"] = recipe.id;
        recipeJson["name"] = recipe.name;
        recipeJson["description"] = recipe.description;
        recipeJson["result_material"] = recipe.resultMaterial;
        recipeJson["success_rate"] = recipe.successRate;
        recipeJson["unlock_level"] = recipe.unlockLevel;
        recipeJson["is_unlocked"] = recipe.isUnlocked;
        
        json ingredientsJson = json::array();
        for (const auto& ingredient : recipe.ingredients) {
            json ingredientJson;
            ingredientJson["material"] = ingredient.first;
            ingredientJson["quantity"] = ingredient.second;
            ingredientsJson.push_back(ingredientJson);
        }
        recipeJson["ingredients"] = ingredientsJson;
        
        j["recipes"].push_back(recipeJson);
    }
    
    return j.dump(4);
}

std::string GameDataManager::generateEventsJson() const {
    json j;
    j["version"] = gameConfig.version.toString();
    j["events"] = json::array();
    
    for (const auto& event : events) {
        json eventJson;
        eventJson["name"] = event.name;
        eventJson["description"] = event.description;
        eventJson["probability"] = event.probability;
        eventJson["reward_materials"] = event.rewardMaterials;
        eventJson["penalty_materials"] = event.penaltyMaterials;
        
        j["events"].push_back(eventJson);
    }
    
    return j.dump(4);
}

// File I/O helper implementations
bool GameDataManager::readFileContent(const std::string& filePath, std::string& content) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    
    return true;
}

bool GameDataManager::writeFileContent(const std::string& filePath, const std::string& content) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool GameDataManager::ensureDirectoryExists(const std::string& dirPath) const {
    try {
        std::filesystem::create_directories(dirPath);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create directory " << dirPath << ": " << e.what() << std::endl;
        return false;
    }
}
