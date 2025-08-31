#include "CraftingSystem.h"
#include "Inventory.h"
#include "Constants.h"
#include <algorithm>
#include <random>
#include <iostream>

CraftingSystem::CraftingSystem() {
    initializeDefaultRecipes();
}

CraftingResult CraftingSystem::craftItem(const Recipe& recipe, Inventory& inventory) {
    // Check if crafting is possible
    if (!canCraft(recipe, inventory)) {
        return CraftingResult(false, Card("", 1, CardType::MISC), "Insufficient materials or recipe not unlocked");
    }
    
    // Calculate actual success rate
    float actualSuccessRate = calculateActualSuccessRate(recipe, inventory);
    
    // Perform random check
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    bool success = dist(gen) <= actualSuccessRate;
    
    // Consume materials
    consumeMaterials(recipe, inventory);

    if (success) {
        // Crafting succeeded
        Card resultCard = recipe.result;
        inventory.addCard(resultCard);

        std::string successMsg = "Successfully crafted " + resultCard.name + "!";
        std::cout << successMsg << std::endl;

        return CraftingResult(true, resultCard, successMsg);
    } else {
        // Crafting failed, may get scrap
        std::string failMsg = "Crafting failed! Materials were wasted.";

        // 50% chance to get scrap
        if (dist(gen) <= 0.5f) {
            Card scrap("Scrap", 1, CardType::MISC);
            scrap.setAttribute(AttributeType::CRAFTING_VALUE, 1.0f);
            scrap.setAttribute(AttributeType::TRADE_VALUE, 2.0f);
            inventory.addCard(scrap);
            failMsg += " But you received some scrap.";
        }
        
        std::cout << failMsg << std::endl;
        return CraftingResult(false, Card("", 1, CardType::MISC), failMsg);
    }
}

bool CraftingSystem::canCraft(const Recipe& recipe, const Inventory& inventory) const {
    return isRecipeUnlocked(recipe.id) && hasEnoughMaterials(recipe, inventory);
}

const std::vector<Recipe>& CraftingSystem::getAllRecipes() const {
    return recipes;
}

const std::vector<Recipe> CraftingSystem::getAvailableRecipes(const Inventory& inventory) const {
    std::vector<Recipe> available;
    
    for (const auto& recipe : recipes) {
        if (canCraft(recipe, inventory)) {
            available.push_back(recipe);
        }
    }
    
    return available;
}

void CraftingSystem::unlockRecipe(const std::string& recipeId) {
    auto it = recipeIndexMap.find(recipeId);
    if (it != recipeIndexMap.end()) {
        recipes[it->second].isUnlocked = true;
        std::cout << "Unlocked new recipe: " << recipes[it->second].name << std::endl;
    }
}

bool CraftingSystem::isRecipeUnlocked(const std::string& recipeId) const {
    auto it = recipeIndexMap.find(recipeId);
    return it != recipeIndexMap.end() && recipes[it->second].isUnlocked;
}

const Recipe* CraftingSystem::getRecipe(const std::string& recipeId) const {
    auto it = recipeIndexMap.find(recipeId);
    return (it != recipeIndexMap.end()) ? &recipes[it->second] : nullptr;
}

void CraftingSystem::initializeDefaultRecipes() {
    recipes.clear();
    recipeIndexMap.clear();
    
    // Basic crafting recipes
    
    // 1. Medkit = Medicine x1 + Bandage x1
    Recipe medkitRecipe("medkit", "Medkit", "A basic medical supply made from medicine and bandage",
        {
            {Constants::CardFactory::createMedicine(), 1},
            {Constants::CardFactory::createBandage(), 1}
        },
        []() {
            Card medkit("Medkit", 2, CardType::HERB);
            medkit.setAttribute(AttributeType::HEALING, 75.0f);
            medkit.setAttribute(AttributeType::WEIGHT, 0.3f);
            medkit.setAttribute(AttributeType::TRADE_VALUE, 45.0f);
            return medkit;
        }(),
        0.9f);
    
    // 2. Enhanced Weapon = Weapon x1 + Metal x1
    Recipe enhancedWeaponRecipe("enhanced_weapon", "Enhanced Weapon", "A weapon reinforced with metal, higher attack power",
        {
            {Constants::CardFactory::createWeapon(), 1},
            {Constants::CardFactory::createMetal(), 1}
        },
        []() {
            Card enhancedWeapon("Enhanced Weapon", 3, CardType::WEAPON);
            enhancedWeapon.setAttribute(AttributeType::ATTACK, 70.0f);
            enhancedWeapon.setAttribute(AttributeType::DURABILITY, 120.0f);
            enhancedWeapon.setAttribute(AttributeType::WEIGHT, 4.0f);
            enhancedWeapon.setAttribute(AttributeType::TRADE_VALUE, 80.0f);
            return enhancedWeapon;
        }(),
        0.8f);
    
    // 3. Wall = Wood x2 + Metal x1
    Recipe wallRecipe("wall", "Wall", "Basic defensive structure",
        {
            {Constants::CardFactory::createWood(), 2},
            {Constants::CardFactory::createMetal(), 1}
        },
        []() {
            Card wall("Wall", 2, CardType::BUILDING);
            wall.setAttribute(AttributeType::DEFENSE, 50.0f);
            wall.setAttribute(AttributeType::DURABILITY, 200.0f);
            wall.setAttribute(AttributeType::WEIGHT, 10.0f);
            wall.setAttribute(AttributeType::TRADE_VALUE, 35.0f);
            return wall;
        }(),
        0.95f);
    
    // 4. Fuel = Wood x1 + Coal x1
    Recipe fuelRecipe("fuel", "Efficient Fuel", "Composite fuel with higher burning efficiency",
        {
            {Constants::CardFactory::createWood(), 1},
            {Constants::CardFactory::createCoal(), 1}
        },
        []() {
            Card fuel("Efficient Fuel", 2, CardType::FUEL);
            fuel.setAttribute(AttributeType::BURN_VALUE, 50.0f);
            fuel.setAttribute(AttributeType::WEIGHT, 1.0f);
            fuel.setAttribute(AttributeType::TRADE_VALUE, 25.0f);
            return fuel;
        }(),
        0.9f);
    
    // 5. Toolbox = Metal x2 + Weapon x1 (disassembly tool)
    Recipe toolboxRecipe("toolbox", "Toolbox", "Multi-functional tool set",
        {
            {Constants::CardFactory::createMetal(), 2},
            {Constants::CardFactory::createWeapon(), 1}
        },
        []() {
            Card toolbox("Toolbox", 3, CardType::TOOL);
            toolbox.setAttribute(AttributeType::CRAFTING_VALUE, 60.0f);
            toolbox.setAttribute(AttributeType::DURABILITY, 150.0f);
            toolbox.setAttribute(AttributeType::WEIGHT, 5.0f);
            toolbox.setAttribute(AttributeType::TRADE_VALUE, 70.0f);
            return toolbox;
        }(),
        0.75f);
    
    // 6. Nutrition Meal = Food x2 + Water x1
    Recipe nutritionMealRecipe("nutrition_meal", "Nutrition Meal", "Nutritious food combination",
        {
            {Constants::CardFactory::createFood(), 2},
            {Constants::CardFactory::createWater(), 1}
        },
        []() {
            Card nutritionMeal("Nutrition Meal", 2, CardType::FOOD);
            nutritionMeal.setAttribute(AttributeType::NUTRITION, 50.0f);
            nutritionMeal.setAttribute(AttributeType::WEIGHT, 1.0f);
            nutritionMeal.setAttribute(AttributeType::TRADE_VALUE, 20.0f);
            return nutritionMeal;
        }(),
        0.95f);
    
    // Add all recipes
    std::vector<Recipe> defaultRecipes = {
        medkitRecipe, enhancedWeaponRecipe, wallRecipe, 
        fuelRecipe, toolboxRecipe, nutritionMealRecipe
    };
    
    for (size_t i = 0; i < defaultRecipes.size(); ++i) {
        recipes.push_back(defaultRecipes[i]);
        recipeIndexMap[defaultRecipes[i].id] = i;
    }
    
    std::cout << "Initialized " << recipes.size() << " crafting recipes" << std::endl;
}

bool CraftingSystem::hasEnoughMaterials(const Recipe& recipe, const Inventory& inventory) const {
    const auto& cards = inventory.getCards();
    
    for (const auto& ingredient : recipe.ingredients) {
        const Card& requiredCard = ingredient.first;
        int requiredQuantity = ingredient.second;
        
        // Find the corresponding card and check the quantity
        bool found = false;
        for (const auto& card : cards) {
            if (card.name == requiredCard.name && 
                card.rarity == requiredCard.rarity &&
                card.quantity >= requiredQuantity) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            return false;
        }
    }
    
    return true;
}

void CraftingSystem::consumeMaterials(const Recipe& recipe, Inventory& inventory) {
    for (const auto& ingredient : recipe.ingredients) {
        const Card& requiredCard = ingredient.first;
        int requiredQuantity = ingredient.second;
        
        // Remove the required quantity of materials
        for (int i = 0; i < requiredQuantity; ++i) {
            inventory.removeCard(requiredCard.name, requiredCard.rarity);
        }
    }
}

float CraftingSystem::calculateActualSuccessRate(const Recipe& recipe, const Inventory& inventory) const {
    float baseRate = recipe.successRate;
    
    // Adjust success rate based on material quality
    float qualityBonus = 0.0f;
    const auto& cards = inventory.getCards();
    
    for (const auto& ingredient : recipe.ingredients) {
        const Card& requiredCard = ingredient.first;
        
        for (const auto& card : cards) {
            if (card.name == requiredCard.name) {
                // Higher rarity slightly increases success rate
                if (card.rarity == 2) qualityBonus += 0.05f;
                else if (card.rarity == 3) qualityBonus += 0.1f;
                break;
            }
        }
    }
    
    return std::min(1.0f, baseRate + qualityBonus);
}
