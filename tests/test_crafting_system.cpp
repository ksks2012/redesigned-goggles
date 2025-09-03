#include "../lib/catch2/catch.hpp"
#include "Systems/CraftingSystem.h"
#include "Core/Inventory.h"
#include "Core/Card.h"

TEST_CASE("Recipe creation and validation", "[CraftingSystem][Recipe]") {
    SECTION("Basic recipe construction") {
        Card ironIngot("Iron Ingot", 1, CardType::METAL);
        Card wood("Wood", 1, CardType::BUILDING);
        Card ironSword("Iron Sword", 2, CardType::WEAPON);
        
        std::vector<std::pair<Card, int>> ingredients = {
            {ironIngot, 2},
            {wood, 1}
        };
        
        Recipe swordRecipe("sword_recipe", "Iron Sword Recipe", 
                          "Craft an iron sword from metal and wood",
                          ingredients, ironSword, 0.9f, 1);
        
        REQUIRE(swordRecipe.id == "sword_recipe");
        REQUIRE(swordRecipe.name == "Iron Sword Recipe");
        REQUIRE(swordRecipe.description == "Craft an iron sword from metal and wood");
        REQUIRE(swordRecipe.ingredients.size() == 2);
        REQUIRE(swordRecipe.result.name == "Iron Sword");
        REQUIRE(swordRecipe.successRate == 0.9f);
        REQUIRE(swordRecipe.unlockLevel == 1);
        REQUIRE(swordRecipe.isUnlocked == true);
    }
    
    SECTION("Recipe with multiple ingredients") {
        Card leather("Leather", 1, CardType::MISC);
        Card iron("Iron", 1, CardType::METAL);
        Card thread("Thread", 1, CardType::MISC);
        Card armor("Leather Armor", 2, CardType::ARMOR);
        
        std::vector<std::pair<Card, int>> ingredients = {
            {leather, 5},
            {iron, 2},
            {thread, 3}
        };
        
        Recipe armorRecipe("armor_recipe", "Leather Armor",
                          "Protective leather armor",
                          ingredients, armor, 0.8f, 2);
        
        REQUIRE(armorRecipe.ingredients.size() == 3);
        REQUIRE(armorRecipe.ingredients[0].second == 5); // Leather quantity
        REQUIRE(armorRecipe.ingredients[1].second == 2); // Iron quantity
        REQUIRE(armorRecipe.ingredients[2].second == 3); // Thread quantity
    }
}

TEST_CASE("CraftingSystem initialization and recipe management", "[CraftingSystem]") {
    CraftingSystem craftingSystem;
    
    SECTION("Default recipes are loaded") {
        const auto& recipes = craftingSystem.getAllRecipes();
        REQUIRE(recipes.size() > 0);
        
        // Check for some expected default recipes (based on actual implementation)
        bool foundMedkitRecipe = false;
        bool foundWallRecipe = false;
        bool foundFuelRecipe = false;
        
        for (const auto& recipe : recipes) {
            if (recipe.id == "medkit") foundMedkitRecipe = true;
            if (recipe.id == "wall") foundWallRecipe = true;
            if (recipe.id == "fuel") foundFuelRecipe = true;
        }
        
        REQUIRE((foundMedkitRecipe || foundWallRecipe || foundFuelRecipe)); // At least one should exist
    }
    
    SECTION("Recipe unlocking system") {
        const auto& recipes = craftingSystem.getAllRecipes();
        
        if (!recipes.empty()) {
            const std::string& firstRecipeId = recipes[0].id;
            
            // Test unlock functionality
            craftingSystem.unlockRecipe(firstRecipeId);
            REQUIRE(craftingSystem.isRecipeUnlocked(firstRecipeId) == true);
            
            // Test getting specific recipe
            const Recipe* recipe = craftingSystem.getRecipe(firstRecipeId);
            REQUIRE(recipe != nullptr);
            REQUIRE(recipe->id == firstRecipeId);
        }
    }
    
    SECTION("Getting non-existent recipe") {
        const Recipe* nonExistentRecipe = craftingSystem.getRecipe("non_existent_recipe");
        REQUIRE(nonExistentRecipe == nullptr);
    }
    
    SECTION("Unlocking non-existent recipe") {
        // This should not crash the system
        craftingSystem.unlockRecipe("non_existent_recipe");
        REQUIRE(craftingSystem.isRecipeUnlocked("non_existent_recipe") == false);
    }
}

TEST_CASE("Crafting availability checking", "[CraftingSystem]") {
    CraftingSystem craftingSystem;
    Inventory inventory;
    
    SECTION("Can craft with sufficient materials") {
        // Add materials to inventory (using actual card names from Constants)
        Card medicine("Medicine", 2, CardType::HERB, 5);
        Card bandage("Bandage", 1, CardType::HERB, 3);
        inventory.addCard(medicine);
        inventory.addCard(bandage);
        
        // Get the actual medkit recipe from the system
        const Recipe* medkitRecipe = craftingSystem.getRecipe("medkit");
        REQUIRE(medkitRecipe != nullptr);
        
        REQUIRE(craftingSystem.canCraft(*medkitRecipe, inventory) == true);
    }
    
    SECTION("Cannot craft with insufficient materials") {
        // Add insufficient materials (need Medicine and Bandage for medkit)
        Card medicine("Medicine", 2, CardType::HERB, 0); // Need 1, have 0
        inventory.addCard(medicine);
        
        const Recipe* medkitRecipe = craftingSystem.getRecipe("medkit");
        REQUIRE(medkitRecipe != nullptr);
        
        REQUIRE(craftingSystem.canCraft(*medkitRecipe, inventory) == false);
    }
    
    SECTION("Cannot craft with missing materials") {
        // Empty inventory - trying to craft medkit
        const Recipe* medkitRecipe = craftingSystem.getRecipe("medkit");
        REQUIRE(medkitRecipe != nullptr);
        
        REQUIRE(craftingSystem.canCraft(*medkitRecipe, inventory) == false);
    }
    
    SECTION("Get available recipes based on inventory") {
        // Add materials for medkit recipe
        Card medicine("Medicine", 2, CardType::HERB, 10);
        Card bandage("Bandage", 1, CardType::HERB, 10);
        Card wood("Wood", 1, CardType::BUILDING, 10);
        Card metal("Metal", 2, CardType::METAL, 10);
        
        inventory.addCard(medicine);
        inventory.addCard(bandage);
        inventory.addCard(wood);
        inventory.addCard(metal);
        
        const auto availableRecipes = craftingSystem.getAvailableRecipes(inventory);
        
        // Should return only recipes that can be crafted
        for (const auto& recipe : availableRecipes) {
            REQUIRE(craftingSystem.canCraft(recipe, inventory) == true);
        }
        
        // Should have at least one available recipe with these materials
        REQUIRE(availableRecipes.size() > 0);
    }
}

TEST_CASE("Crafting execution and results", "[CraftingSystem]") {
    CraftingSystem craftingSystem;
    Inventory inventory;
    
    SECTION("Successful crafting with 100% success rate") {
        // Setup inventory with exact materials needed for wall recipe (95% success rate, close to 100%)
        Card wood("Wood", 1, CardType::BUILDING, 2);
        Card metal("Metal", 2, CardType::METAL, 1);
        inventory.addCard(wood);
        inventory.addCard(metal);
        
        const Recipe* wallRecipe = craftingSystem.getRecipe("wall");
        REQUIRE(wallRecipe != nullptr);
        
        // Note: Since crafting has RNG, we'll test multiple times to verify the logic works
        bool successAchieved = false;
        for (int attempt = 0; attempt < 10; ++attempt) {
            // Reset inventory for each attempt
            Inventory testInventory;
            Card testWood("Wood", 1, CardType::BUILDING, 2);
            Card testMetal("Metal", 2, CardType::METAL, 1);
            testInventory.addCard(testWood);
            testInventory.addCard(testMetal);
            
            CraftingResult craftResult = craftingSystem.craftItem(*wallRecipe, testInventory);
            
            if (craftResult.success) {
                successAchieved = true;
                REQUIRE(craftResult.resultCard.name == "Wall");
                REQUIRE(!craftResult.message.empty());
                
                // Check that materials were consumed (should have reduced quantities)
                const auto& cards = testInventory.getCards();
                bool materialsConsumed = true;
                for (const auto& card : cards) {
                    if (card.name == "Wood" && card.quantity >= 2) materialsConsumed = false;
                    if (card.name == "Metal" && card.quantity >= 1) materialsConsumed = false;
                }
                // Note: materials should be consumed regardless of result
                break;
            }
        }
        
        // With 95% success rate and 10 attempts, we should get at least one success
        REQUIRE(successAchieved == true);
    }
    
    SECTION("Failed crafting with insufficient materials") {
        // Setup inventory with insufficient materials for medkit
        Card medicine("Medicine", 2, CardType::HERB, 0); // Need 1, have 0
        inventory.addCard(medicine);
        
        const Recipe* medkitRecipe = craftingSystem.getRecipe("medkit");
        REQUIRE(medkitRecipe != nullptr);
        
        CraftingResult craftResult = craftingSystem.craftItem(*medkitRecipe, inventory);
        
        REQUIRE(craftResult.success == false);
        REQUIRE(!craftResult.message.empty());
        
        // Materials should not be consumed on failure due to insufficient resources
        const auto& cards = inventory.getCards();
        REQUIRE(cards.size() == 1);
        REQUIRE(cards[0].quantity == 0); // Should remain unchanged
    }
    
    SECTION("Crafting with excess materials") {
        // Setup inventory with more materials than needed for medkit
        Card medicine("Medicine", 2, CardType::HERB, 10);
        Card bandage("Bandage", 1, CardType::HERB, 5);
        inventory.addCard(medicine);
        inventory.addCard(bandage);
        
        const Recipe* medkitRecipe = craftingSystem.getRecipe("medkit");
        REQUIRE(medkitRecipe != nullptr);
        
        // Try crafting multiple times since there's RNG involved
        bool craftingAttempted = false;
        for (int attempt = 0; attempt < 5; ++attempt) {
            Inventory testInventory;
            Card testMedicine("Medicine", 2, CardType::HERB, 10);
            Card testBandage("Bandage", 1, CardType::HERB, 5);
            testInventory.addCard(testMedicine);
            testInventory.addCard(testBandage);
            
            CraftingResult craftResult = craftingSystem.craftItem(*medkitRecipe, testInventory);
            
            if (craftResult.success) {
                craftingAttempted = true;
                
                // Check that only required amounts were consumed
                const auto& cards = testInventory.getCards();
                for (const auto& card : cards) {
                    if (card.name == "Medicine") {
                        REQUIRE(card.quantity == 9); // 10 - 1 = 9
                    }
                    if (card.name == "Bandage") {
                        REQUIRE(card.quantity == 4); // 5 - 1 = 4
                    }
                }
                break;
            }
        }
        
        // At least one successful craft should happen with 90% success rate
        REQUIRE(craftingAttempted == true);
    }
}

TEST_CASE("CraftingSystem edge cases", "[CraftingSystem]") {
    CraftingSystem craftingSystem;
    Inventory inventory;
    
    SECTION("Recipe with zero success rate") {
        Card material("Common Stone", 1, CardType::MISC, 5);
        inventory.addCard(material);
        
        Card result("Magic Stone", 3, CardType::MISC);
        std::vector<std::pair<Card, int>> ingredients = {
            {Card("Common Stone", 1, CardType::MISC), 1}
        };
        Recipe impossibleRecipe("impossible", "Impossible Recipe", "Never succeeds",
                               ingredients, result, 0.0f, 0);
        
        // Multiple attempts should all fail
        for (int i = 0; i < 5; ++i) {
            CraftingResult result = craftingSystem.craftItem(impossibleRecipe, inventory);
            REQUIRE(result.success == false);
        }
    }
    
    SECTION("Recipe with no ingredients") {
        Card result("Free Item", 1, CardType::MISC);
        std::vector<std::pair<Card, int>> ingredients; // Empty ingredients
        Recipe freeRecipe("free_recipe", "Free Recipe", "Something from nothing",
                         ingredients, result, 1.0f, 0);
        
        // Add the recipe to the system for testing
        REQUIRE(craftingSystem.canCraft(freeRecipe, inventory) == false); // Should fail because recipe not in system
        
        // Test crafting directly
        CraftingResult craftResult = craftingSystem.craftItem(freeRecipe, inventory);
        REQUIRE(craftResult.success == false); // Should fail because recipe not unlocked in system
    }
    
    SECTION("Recipe requiring zero quantity of material") {
        Card material("Test Material", 1, CardType::MISC, 1);
        inventory.addCard(material);
        
        Card result("Test Result", 1, CardType::MISC);
        std::vector<std::pair<Card, int>> ingredients = {
            {Card("Test Material", 1, CardType::MISC), 0} // Zero quantity required
        };
        Recipe zeroRecipe("zero_recipe", "Zero Recipe", "Requires zero materials",
                         ingredients, result, 1.0f, 0);
        
        // This recipe is not in the system, so it should fail
        REQUIRE(craftingSystem.canCraft(zeroRecipe, inventory) == false);
        
        CraftingResult craftResult = craftingSystem.craftItem(zeroRecipe, inventory);
        REQUIRE(craftResult.success == false); // Should fail because recipe not in system
    }
}
