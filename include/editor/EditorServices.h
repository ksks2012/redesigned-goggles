#ifndef EDITOR_SERVICES_H
#define EDITOR_SERVICES_H

#include "EditorInterfaces.h"
#include "DataManager.h"
#include "Game.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace Editor {

/**
 * Console-based user interface implementation
 * Follows Single Responsibility Principle (SRP) - only handles UI
 */
class ConsoleUserInterface : public IUserInterface {
public:
    void displayMessage(const std::string& message) override {
        std::cout << message << std::endl;
    }
    
    void displayError(const std::string& error) override {
        std::cout << "✗ Error: " << error << std::endl;
    }
    
    void displayWarning(const std::string& warning) override {
        std::cout << "⚠ Warning: " << warning << std::endl;
    }
    
    void displaySuccess(const std::string& success) override {
        std::cout << "✓ " << success << std::endl;
    }
    
    void displayTable(const std::vector<std::vector<std::string>>& data,
                     const std::vector<std::string>& headers) override {
        if (headers.empty() || data.empty()) return;
        
        // Calculate column widths
        std::vector<size_t> widths(headers.size(), 0);
        for (size_t i = 0; i < headers.size(); ++i) {
            widths[i] = headers[i].length();
        }
        
        for (const auto& row : data) {
            for (size_t i = 0; i < row.size() && i < widths.size(); ++i) {
                widths[i] = std::max(widths[i], row[i].length());
            }
        }
        
        // Display headers
        for (size_t i = 0; i < headers.size(); ++i) {
            std::cout << std::left << std::setw(widths[i] + 2) << headers[i];
        }
        std::cout << std::endl;
        
        // Display separator
        for (size_t width : widths) {
            std::cout << std::string(width + 2, '-');
        }
        std::cout << std::endl;
        
        // Display data
        for (const auto& row : data) {
            for (size_t i = 0; i < row.size() && i < widths.size(); ++i) {
                std::cout << std::left << std::setw(widths[i] + 2) << row[i];
            }
            std::cout << std::endl;
        }
    }
    
    std::string getUserInput(const std::string& prompt) override {
        std::cout << prompt << ": ";
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
    
    int getUserChoice(const std::string& prompt, int min, int max) override {
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
            
            displayError("Invalid input. Please enter a number between " + 
                        std::to_string(min) + " and " + std::to_string(max));
        }
    }
    
    float getUserFloat(const std::string& prompt) override {
        while (true) {
            std::cout << prompt << ": ";
            std::string input;
            std::getline(std::cin, input);
            
            try {
                return std::stof(input);
            } catch (const std::exception&) {
                displayError("Invalid input. Please enter a valid number.");
            }
        }
    }
    
    bool confirmAction(const std::string& action) override {
        std::cout << "Are you sure you want to " << action << "? (y/N): ";
        std::string input;
        std::getline(std::cin, input);
        return input == "y" || input == "Y" || input == "yes" || input == "Yes";
    }
};

/**
 * Data service implementation using DataManager
 * Follows Single Responsibility Principle (SRP) - only handles data operations
 */
class DataManagerService : public IDataService {
private:
    DataManagement::GameDataManager& dataManager_;
    
public:
    explicit DataManagerService(DataManagement::GameDataManager& dataManager)
        : dataManager_(dataManager) {}
    
    // Material operations
    const std::vector<DataManagement::MaterialData>& getMaterials() const override {
        return dataManager_.getMaterials();
    }
    
    DataManagement::MaterialData* findMaterial(const std::string& name, int rarity) override {
        return dataManager_.findMaterial(name, rarity);
    }
    
    bool addMaterial(const DataManagement::MaterialData& material) override {
        auto materials = dataManager_.getMaterials();
        materials.push_back(material);
        dataManager_.setMaterials(materials);
        return true;
    }
    
    bool updateMaterial(const std::string& name, int rarity, 
                       const DataManagement::MaterialData& material) override {
        auto* existingMaterial = findMaterial(name, rarity);
        if (existingMaterial) {
            *existingMaterial = material;
            return true;
        }
        return false;
    }
    
    bool removeMaterial(const std::string& name, int rarity) override {
        auto materials = dataManager_.getMaterials();
        auto it = std::find_if(materials.begin(), materials.end(),
                              [&](const DataManagement::MaterialData& m) {
                                  return m.name == name && m.rarity == rarity;
                              });
        
        if (it != materials.end()) {
            materials.erase(it);
            dataManager_.setMaterials(materials);
            return true;
        }
        return false;
    }
    
    // Recipe operations
    const std::vector<DataManagement::RecipeData>& getRecipes() const override {
        return dataManager_.getRecipes();
    }
    
    DataManagement::RecipeData* findRecipe(const std::string& id) override {
        return dataManager_.findRecipe(id);
    }
    
    bool addRecipe(const DataManagement::RecipeData& recipe) override {
        auto recipes = dataManager_.getRecipes();
        recipes.push_back(recipe);
        dataManager_.setRecipes(recipes);
        return true;
    }
    
    bool updateRecipe(const std::string& id, const DataManagement::RecipeData& recipe) override {
        auto* existingRecipe = findRecipe(id);
        if (existingRecipe) {
            *existingRecipe = recipe;
            return true;
        }
        return false;
    }
    
    bool removeRecipe(const std::string& id) override {
        auto recipes = dataManager_.getRecipes();
        auto it = std::find_if(recipes.begin(), recipes.end(),
                              [&](const DataManagement::RecipeData& r) {
                                  return r.id == id;
                              });
        
        if (it != recipes.end()) {
            recipes.erase(it);
            dataManager_.setRecipes(recipes);
            return true;
        }
        return false;
    }
    
    // Event operations
    const std::vector<DataManagement::EventData>& getEvents() const override {
        return dataManager_.getEvents();
    }
    
    bool addEvent(const DataManagement::EventData& event) override {
        auto events = dataManager_.getEvents();
        events.push_back(event);
        dataManager_.setEvents(events);
        return true;
    }
    
    bool removeEvent(const std::string& name) override {
        auto events = dataManager_.getEvents();
        auto it = std::find_if(events.begin(), events.end(),
                              [&](const DataManagement::EventData& e) {
                                  return e.name == name;
                              });
        
        if (it != events.end()) {
            events.erase(it);
            dataManager_.setEvents(events);
            return true;
        }
        return false;
    }
    
    // Data validation and persistence
    DataManagement::ValidationResult validateAll() const override {
        return dataManager_.validateAll();
    }
    
    bool saveAll() override {
        return dataManager_.saveAllData();
    }
    
    bool loadAll() override {
        return dataManager_.loadAllData();
    }
    
    bool exportData(const std::string& directory) override {
        return dataManager_.saveAllData(directory + "/");
    }
};

/**
 * Game state service implementation
 * Follows Single Responsibility Principle (SRP) - only handles game state
 */
class GameStateService : public IGameStateService {
private:
    Game* gameInstance_;
    DataManagement::GameDataManager& dataManager_;
    
public:
    GameStateService(Game* gameInstance, DataManagement::GameDataManager& dataManager)
        : gameInstance_(gameInstance), dataManager_(dataManager) {}
    
    bool isGameConnected() const override {
        return gameInstance_ != nullptr;
    }
    
    std::vector<std::string> getInventoryItems() const override {
        std::vector<std::string> items;
        if (!gameInstance_) return items;
        
        const auto& inventory = gameInstance_->getInventory();
        auto cards = inventory.getCards();
        
        for (const auto& card : cards) {
            items.push_back(card.name + " x" + std::to_string(card.quantity));
        }
        
        return items;
    }
    
    std::vector<std::string> getUnlockedRecipes() const override {
        std::vector<std::string> recipes;
        if (!gameInstance_) return recipes;
        
        const auto& craftingSystem = gameInstance_->getCraftingSystem();
        auto allRecipes = craftingSystem.getAllRecipes();
        
        for (const auto& recipe : allRecipes) {
            std::string status = recipe.isUnlocked ? "Unlocked" : "Locked";
            recipes.push_back(recipe.name + " - " + status);
        }
        
        return recipes;
    }
    
    bool syncFromGame() override {
        if (!gameInstance_) return false;
        
        // Load current data from files
        return dataManager_.loadAllData();
    }
    
    bool syncToGame() override {
        if (!gameInstance_) return false;
        
        try {
            // Get game systems
            auto& inventory = gameInstance_->getInventory();
            auto& craftingSystem = gameInstance_->getCraftingSystem();
            auto& controller = gameInstance_->getController();
            
            // Apply data to game systems
            bool success = true;
            success &= dataManager_.applyToInventory(inventory);
            success &= dataManager_.applyToCraftingSystem(craftingSystem);
            success &= dataManager_.applyToController(controller);
            
            return success;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    std::string getGameStatusSummary() const override {
        std::ostringstream oss;
        
        const auto& config = dataManager_.getGameConfig();
        const auto& materials = dataManager_.getMaterials();
        const auto& recipes = dataManager_.getRecipes();
        const auto& events = dataManager_.getEvents();
        
        oss << "Game Configuration:" << std::endl;
        oss << "  Version: " << config.version.toString() << std::endl;
        oss << "  Name: " << config.configName << std::endl;
        oss << std::endl;
        oss << "Data Counts:" << std::endl;
        oss << "  Materials: " << materials.size() << std::endl;
        oss << "  Recipes: " << recipes.size() << std::endl;
        oss << "  Events: " << events.size() << std::endl;
        oss << std::endl;
        oss << "Game Instance: " << (gameInstance_ ? "Connected" : "Not connected");
        
        return oss.str();
    }
};

} // namespace Editor

#endif // EDITOR_SERVICES_H
