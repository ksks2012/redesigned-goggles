#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Card.h"

// Forward declarations
namespace DataManagement {
    class GameDataManager;
}

// Recipe structure
struct Recipe {
    std::string id;                                    // Unique recipe identifier
    std::string name;                                  // Recipe name
    std::string description;                           // Recipe description
    std::vector<std::pair<Card, int>> ingredients;     // Required materials (Card, quantity)
    Card result;                                       // Crafting result
    float successRate;                                 // Success rate (0.0 - 1.0)
    int unlockLevel;                                   // Unlock level (future feature)
    bool isUnlocked;                                   // Whether unlocked
    
    Recipe(const std::string& recipeId, const std::string& recipeName, 
           const std::string& desc, const std::vector<std::pair<Card, int>>& ing,
           const Card& res, float success = 1.0f, int level = 0)
        : id(recipeId), name(recipeName), description(desc), ingredients(ing),
          result(res), successRate(success), unlockLevel(level), isUnlocked(true) {}
};

// Crafting result structure
struct CraftingResult {
    bool success;
    Card resultCard;
    std::string message;
    std::vector<Card> wastedMaterials;  // Materials lost on failure
    
    CraftingResult(bool s, const Card& card, const std::string& msg)
        : success(s), resultCard(card), message(msg) {}
};

// Crafting system class
class CraftingSystem {
public:
    CraftingSystem();
    
    // Core functionality
    CraftingResult craftItem(const Recipe& recipe, class Inventory& inventory);
    bool canCraft(const Recipe& recipe, const class Inventory& inventory) const;
    const std::vector<Recipe>& getAllRecipes() const;
    const std::vector<Recipe> getAvailableRecipes(const class Inventory& inventory) const;
    
    // Recipe management
    void unlockRecipe(const std::string& recipeId);
    bool isRecipeUnlocked(const std::string& recipeId) const;
    const Recipe* getRecipe(const std::string& recipeId) const;
    
    // Initialize recipes
    void initializeDefaultRecipes();
    void loadRecipesFromDataManager(const DataManagement::GameDataManager& dataManager);
    void clearRecipes();

private:
    std::vector<Recipe> recipes;
    std::unordered_map<std::string, size_t> recipeIndexMap;  // Fast recipe lookup
    
    // Helper methods
    bool hasEnoughMaterials(const Recipe& recipe, const class Inventory& inventory) const;
    void consumeMaterials(const Recipe& recipe, class Inventory& inventory);
    float calculateActualSuccessRate(const Recipe& recipe, const class Inventory& inventory) const;
};
