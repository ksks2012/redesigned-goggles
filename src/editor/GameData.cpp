#include "editor/GameData.h"
#include "Game.h"
#include "Constants.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// MaterialTemplate implementation
Card MaterialTemplate::toCard(int quantity) const {
    Card card(name, rarity, type, quantity);
    
    // Set attributes
    for (const auto& attr : attributes) {
        card.setAttribute(attr.first, attr.second);
    }
    
    return card;
}

MaterialTemplate MaterialTemplate::fromCard(const Card& card) {
    MaterialTemplate material;
    material.id = card.name; // Use name as ID for now
    material.name = card.name;
    material.type = card.type;
    material.rarity = card.rarity;
    
    // Copy attributes
    for (auto attrType : {AttributeType::WEIGHT, AttributeType::NUTRITION, AttributeType::ATTACK,
                         AttributeType::DEFENSE, AttributeType::HEALING, AttributeType::DURABILITY,
                         AttributeType::BURN_VALUE, AttributeType::CRAFTING_VALUE, AttributeType::TRADE_VALUE}) {
        if (card.hasAttribute(attrType)) {
            material.attributes[attrType] = card.getAttribute(attrType);
        }
    }
    
    return material;
}

// EventCondition implementation
bool EventCondition::evaluate() const {
    // Placeholder implementation - would need game state access
    // This should be connected to the actual game state
    return false;
}

// EventEffect implementation
void EventEffect::execute() const {
    // Placeholder implementation - would need game state access
    // This should be connected to the actual game state
}

// EventTemplate implementation
bool EventTemplate::canTrigger() const {
    for (const auto& condition : conditions) {
        if (!condition.evaluate()) {
            return false;
        }
    }
    return true;
}

void EventTemplate::trigger() const {
    if (!canTrigger()) return;
    
    for (const auto& effect : effects) {
        effect.execute();
    }
}

// GameDataManager implementation
GameDataManager::GameDataManager() : gameInstance_(nullptr) {
    // Initialize with some default materials
    initializeDefaults();
}

GameDataManager::GameDataManager(Game* gameInstance) : gameInstance_(gameInstance) {
    // Initialize with some default materials
    initializeDefaults();
    
    // Sync from game if available
    if (gameInstance_) {
        syncFromGame();
    }
}

void GameDataManager::addMaterial(const MaterialTemplate& material) {
    // Check for duplicate ID
    auto it = std::find_if(materials_.begin(), materials_.end(),
        [&material](const MaterialTemplate& m) { return m.id == material.id; });
    
    if (it != materials_.end()) {
        std::cerr << "Warning: Material with ID '" << material.id << "' already exists" << std::endl;
        return;
    }
    
    materials_.push_back(material);
    notifyChange("material", material.id);
}

void GameDataManager::removeMaterial(const std::string& id) {
    auto it = std::find_if(materials_.begin(), materials_.end(),
        [&id](const MaterialTemplate& m) { return m.id == id; });
    
    if (it != materials_.end()) {
        materials_.erase(it);
        notifyChange("material", id);
    }
}

void GameDataManager::updateMaterial(const std::string& id, const MaterialTemplate& material) {
    auto it = std::find_if(materials_.begin(), materials_.end(),
        [&id](const MaterialTemplate& m) { return m.id == id; });
    
    if (it != materials_.end()) {
        *it = material;
        it->id = id; // Ensure ID doesn't change
        notifyChange("material", id);
    }
}

MaterialTemplate* GameDataManager::getMaterial(const std::string& id) {
    auto it = std::find_if(materials_.begin(), materials_.end(),
        [&id](const MaterialTemplate& m) { return m.id == id; });
    
    return (it != materials_.end()) ? &(*it) : nullptr;
}

void GameDataManager::addRecipe(const Recipe& recipe) {
    // Check for duplicate name
    auto it = std::find_if(recipes_.begin(), recipes_.end(),
        [&recipe](const Recipe& r) { return r.name == recipe.name; });
    
    if (it != recipes_.end()) {
        std::cerr << "Warning: Recipe with name '" << recipe.name << "' already exists" << std::endl;
        return;
    }
    
    recipes_.push_back(recipe);
    notifyChange("recipe", recipe.name);
}

void GameDataManager::removeRecipe(const std::string& name) {
    auto it = std::find_if(recipes_.begin(), recipes_.end(),
        [&name](const Recipe& r) { return r.name == name; });
    
    if (it != recipes_.end()) {
        recipes_.erase(it);
        notifyChange("recipe", name);
    }
}

void GameDataManager::updateRecipe(const std::string& name, const Recipe& recipe) {
    auto it = std::find_if(recipes_.begin(), recipes_.end(),
        [&name](const Recipe& r) { return r.name == name; });
    
    if (it != recipes_.end()) {
        *it = recipe;
        notifyChange("recipe", name);
    }
}

Recipe* GameDataManager::getRecipe(const std::string& name) {
    auto it = std::find_if(recipes_.begin(), recipes_.end(),
        [&name](const Recipe& r) { return r.name == name; });
    
    return (it != recipes_.end()) ? &(*it) : nullptr;
}

void GameDataManager::addEvent(const EventTemplate& event) {
    // Check for duplicate ID
    auto it = std::find_if(events_.begin(), events_.end(),
        [&event](const EventTemplate& e) { return e.id == event.id; });
    
    if (it != events_.end()) {
        std::cerr << "Warning: Event with ID '" << event.id << "' already exists" << std::endl;
        return;
    }
    
    events_.push_back(event);
    notifyChange("event", event.id);
}

void GameDataManager::removeEvent(const std::string& id) {
    auto it = std::find_if(events_.begin(), events_.end(),
        [&id](const EventTemplate& e) { return e.id == id; });
    
    if (it != events_.end()) {
        events_.erase(it);
        notifyChange("event", id);
    }
}

void GameDataManager::updateEvent(const std::string& id, const EventTemplate& event) {
    auto it = std::find_if(events_.begin(), events_.end(),
        [&id](const EventTemplate& e) { return e.id == id; });
    
    if (it != events_.end()) {
        *it = event;
        it->id = id; // Ensure ID doesn't change
        notifyChange("event", id);
    }
}

EventTemplate* GameDataManager::getEvent(const std::string& id) {
    auto it = std::find_if(events_.begin(), events_.end(),
        [&id](const EventTemplate& e) { return e.id == id; });
    
    return (it != events_.end()) ? &(*it) : nullptr;
}

GameDataManager::ValidationResult GameDataManager::validateData() const {
    ValidationResult result;
    result.isValid = true;
    
    // Validate material IDs are unique
    std::set<std::string> materialIds;
    for (const auto& material : materials_) {
        if (materialIds.count(material.id)) {
            result.isValid = false;
            result.errors.push_back("Duplicate material ID: " + material.id);
        }
        materialIds.insert(material.id);
        
        if (material.name.empty()) {
            result.warnings.push_back("Material " + material.id + " has empty name");
        }
    }
    
    // Validate recipe references
    for (const auto& recipe : recipes_) {
        for (const auto& ingredient : recipe.ingredients) {
            std::string ingredientId = ingredient.first.name; // Using name as ID
            if (materialIds.find(ingredientId) == materialIds.end()) {
                result.isValid = false;
                result.errors.push_back("Recipe '" + recipe.name + "' references unknown material: " + ingredientId);
            }
        }
        
        // Check result material
        std::string resultId = recipe.result.name;
        if (materialIds.find(resultId) == materialIds.end()) {
            result.warnings.push_back("Recipe '" + recipe.name + "' creates unknown material: " + resultId);
        }
    }
    
    // Validate event IDs are unique
    std::set<std::string> eventIds;
    for (const auto& event : events_) {
        if (eventIds.count(event.id)) {
            result.isValid = false;
            result.errors.push_back("Duplicate event ID: " + event.id);
        }
        eventIds.insert(event.id);
    }
    
    return result;
}

bool GameDataManager::saveToFile(const std::string& filename) const {
    try {
        json j;
        
        // Save materials
        j["materials"] = json::array();
        for (const auto& material : materials_) {
            json matJson;
            matJson["id"] = material.id;
            matJson["name"] = material.name;
            matJson["type"] = static_cast<int>(material.type);
            matJson["rarity"] = material.rarity;
            matJson["description"] = material.description;
            
            matJson["attributes"] = json::object();
            for (const auto& attr : material.attributes) {
                matJson["attributes"][std::to_string(static_cast<int>(attr.first))] = attr.second;
            }
            
            j["materials"].push_back(matJson);
        }
        
        // Save recipes
        j["recipes"] = json::array();
        for (const auto& recipe : recipes_) {
            json recipeJson;
            recipeJson["id"] = recipe.id;
            recipeJson["name"] = recipe.name;
            recipeJson["description"] = recipe.description;
            recipeJson["success_rate"] = recipe.successRate;
            recipeJson["unlock_level"] = recipe.unlockLevel;
            recipeJson["is_unlocked"] = recipe.isUnlocked;
            
            // Save ingredients
            recipeJson["ingredients"] = json::array();
            for (const auto& ingredient : recipe.ingredients) {
                json ingJson;
                ingJson["card_name"] = ingredient.first.name;
                ingJson["quantity"] = ingredient.second;
                recipeJson["ingredients"].push_back(ingJson);
            }
            
            // Save result
            recipeJson["result"] = json::object();
            recipeJson["result"]["name"] = recipe.result.name;
            recipeJson["result"]["type"] = static_cast<int>(recipe.result.type);
            recipeJson["result"]["rarity"] = recipe.result.rarity;
            
            j["recipes"].push_back(recipeJson);
        }
        
        // Save events
        j["events"] = json::array();
        for (const auto& event : events_) {
            json eventJson;
            eventJson["id"] = event.id;
            eventJson["name"] = event.name;
            eventJson["description"] = event.description;
            eventJson["is_repeatable"] = event.isRepeatable;
            eventJson["priority"] = event.priority;
            j["events"].push_back(eventJson);
        }
        
        // Write to file
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return false;
        }
        
        file << j.dump(4); // Pretty print with 4 spaces
        std::cout << "Game data saved to " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving to file: " << e.what() << std::endl;
        return false;
    }
}

bool GameDataManager::loadFromFile(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for reading: " << filename << std::endl;
            return false;
        }
        
        json j;
        file >> j;
        
        // Load materials
        if (j.contains("materials")) {
            materials_.clear();
            for (const auto& matJson : j["materials"]) {
                MaterialTemplate material;
                material.id = matJson["id"];
                material.name = matJson["name"];
                material.type = static_cast<CardType>(matJson["type"]);
                material.rarity = matJson["rarity"];
                if (matJson.contains("description")) {
                    material.description = matJson["description"];
                }
                
                if (matJson.contains("attributes")) {
                    for (const auto& attr : matJson["attributes"].items()) {
                        AttributeType attrType = static_cast<AttributeType>(std::stoi(attr.key()));
                        material.attributes[attrType] = attr.value();
                    }
                }
                
                materials_.push_back(material);
            }
        }
        
        // Load recipes
        if (j.contains("recipes")) {
            recipes_.clear();
            for (const auto& recipeJson : j["recipes"]) {
                // Create basic recipe structure
                std::vector<std::pair<Card, int>> ingredients;
                Card result("placeholder", 1, CardType::MISC, 1);
                
                Recipe recipe(
                    recipeJson["id"],
                    recipeJson["name"],
                    recipeJson.value("description", ""),
                    ingredients,
                    result,
                    recipeJson.value("success_rate", 1.0f),
                    recipeJson.value("unlock_level", 0)
                );
                
                recipes_.push_back(recipe);
            }
        }
        
        // Load events
        if (j.contains("events")) {
            events_.clear();
            for (const auto& eventJson : j["events"]) {
                EventTemplate event;
                event.id = eventJson["id"];
                event.name = eventJson["name"];
                event.description = eventJson.value("description", "");
                event.isRepeatable = eventJson.value("is_repeatable", false);
                event.priority = eventJson.value("priority", 0);
                
                events_.push_back(event);
            }
        }
        
        std::cout << "Game data loaded from " << filename << std::endl;
        std::cout << "Loaded " << materials_.size() << " materials, " 
                  << recipes_.size() << " recipes, " 
                  << events_.size() << " events" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading from file: " << e.what() << std::endl;
        return false;
    }
}

void GameDataManager::pushState() {
    if (undoStack_.size() >= MAX_UNDO_STATES) {
        undoStack_.erase(undoStack_.begin());
    }
    
    undoStack_.push_back(captureCurrentState());
    redoStack_.clear(); // Clear redo stack when new state is pushed
}

bool GameDataManager::undo() {
    if (undoStack_.empty()) {
        return false;
    }
    
    redoStack_.push_back(captureCurrentState());
    DataState previousState = undoStack_.back();
    undoStack_.pop_back();
    
    restoreState(previousState);
    return true;
}

bool GameDataManager::redo() {
    if (redoStack_.empty()) {
        return false;
    }
    
    undoStack_.push_back(captureCurrentState());
    DataState nextState = redoStack_.back();
    redoStack_.pop_back();
    
    restoreState(nextState);
    return true;
}

void GameDataManager::notifyChange(const std::string& type, const std::string& id) {
    if (changeCallback_) {
        changeCallback_(type, id);
    }
}

GameDataManager::DataState GameDataManager::captureCurrentState() const {
    DataState state;
    state.materials = materials_;
    state.recipes = recipes_;
    state.events = events_;
    return state;
}

void GameDataManager::restoreState(const DataState& state) {
    materials_ = state.materials;
    recipes_ = state.recipes;
    events_ = state.events;
}

void GameDataManager::initializeDefaults() {
    // Add some default materials based on existing constants
    for (const auto& cardName : Constants::CARD_NAMES) {
        MaterialTemplate material;
        material.id = cardName;
        material.name = cardName;
        material.type = CardType::MISC; // Default type
        material.rarity = 1; // Common
        material.attributes[AttributeType::WEIGHT] = 1.0f;
        materials_.push_back(material);
    }
}

// Game data synchronization methods
void GameDataManager::syncFromGame() {
    if (!gameInstance_) return;
    
    // Sync materials from current inventory
    materials_.clear();
    const auto& inventory = gameInstance_->getInventory();
    auto inventoryCards = inventory.getCards();
    
    // Create materials from inventory cards
    std::set<std::string> addedMaterials;
    for (const auto& card : inventoryCards) {
        if (addedMaterials.find(card.name) == addedMaterials.end()) {
            MaterialTemplate material = MaterialTemplate::fromCard(card);
            materials_.push_back(material);
            addedMaterials.insert(card.name);
        }
    }
    
    // Sync recipes from crafting system
    recipes_.clear();
    const auto& craftingSystem = gameInstance_->getCraftingSystem();
    recipes_ = craftingSystem.getAllRecipes();
    
    std::cout << "Synced " << materials_.size() << " materials and " 
              << recipes_.size() << " recipes from game" << std::endl;
}

void GameDataManager::syncToGame() {
    if (!gameInstance_) {
        std::cout << "No game instance available for sync" << std::endl;
        return;
    }
    
    // Note: This is more complex as it requires modifying the game state
    // For now, we'll focus on reading from the game
    std::cout << "Sync to game not yet implemented (requires careful state management)" << std::endl;
}

GameDataManager::GameState GameDataManager::getCurrentGameState() const {
    GameState state;
    
    if (!gameInstance_) {
        return state;
    }
    
    // Get inventory cards
    const auto& inventory = gameInstance_->getInventory();
    state.inventoryCards = inventory.getCards();
    
    // Get available recipes
    const auto& craftingSystem = gameInstance_->getCraftingSystem();
    auto recipes = craftingSystem.getAllRecipes();
    for (const auto& recipe : recipes) {
        state.availableRecipes.push_back(recipe.name);
    }
    
    // Note: Player health would need to be added to the game state
    state.playerHealth = 100; // Placeholder
    
    return state;
}
