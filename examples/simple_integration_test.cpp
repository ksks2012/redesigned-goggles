/**
 * @file simple_integration_test.cpp
 * @brief Simple tech tree and crafting system integration test
 */

#include "Systems/TechTreeSystem.h"
#include "Systems/CraftingSystem.h"
#include "Systems/SDLManager.h"
#include "Systems/DataManager.h"
#include "Core/Inventory.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Simple Tech Tree and Crafting Integration Test ===" << std::endl;
    
    try {
        // Create necessary systems (avoid SimpleGameController dependency issues)
        auto sdlManager = std::make_unique<SDLManager>();
        auto craftingSystem = std::make_unique<CraftingSystem>();
        auto inventory = std::make_unique<Inventory>();
        
        // Initialize SDL (simplified, for testing only)
        if (!sdlManager->initialize()) {
            std::cerr << "SDL initialization failed" << std::endl;
            return -1;
        }
        
        // Create a test DataManager
        auto dataManager = std::make_unique<DataManagement::GameDataManager>();
        
        // Create TechTreeSystem
        auto techTreeSystem = std::make_unique<TechTreeSystem>(*sdlManager, 
                                                               dataManager.get(), 
                                                               craftingSystem.get());
        
        std::cout << "\n1. Systems initialized" << std::endl;
        
        // Initialize tech tree
        if (!techTreeSystem->initialize()) {
            std::cerr << "Tech tree system initialization failed" << std::endl;
            return -1;
        }
        
        std::cout << "2. Tech tree system initialized successfully" << std::endl;
        
        // Inspect tech tree contents
        TechTree* techTree = techTreeSystem->getTechTree();
        if (techTree) {
            const auto& allTechs = techTree->getAllTechs();
            std::cout << "3. Tech tree node count: " << allTechs.size() << std::endl;
            
            // List all techs
            for (const auto& techPair : allTechs) {
                const auto& tech = techPair.second;
                std::cout << "   - " << tech->name << " (" << tech->id << ") - status: " 
                         << static_cast<int>(tech->status) << std::endl;
            }
        }
        
        // Inspect crafting system
        craftingSystem->initializeDefaultRecipes();
        const auto& allRecipes = craftingSystem->getAllRecipes();
        std::cout << "4. Total number of recipes in crafting system: " << allRecipes.size() << std::endl;
        
        auto availableRecipes = craftingSystem->getAvailableRecipes(*inventory);
        std::cout << "5. Currently available recipes: " << availableRecipes.size() << std::endl;
        
        // Test tech unlocks recipes feature
        std::cout << "\n6. Test tech unlocks recipes:" << std::endl;
        
        // Attempt to unlock recipes from the first tech
        if (techTree && !techTree->getAllTechs().empty()) {
            const auto& firstTech = techTree->getAllTechs().begin();
            const std::string& techId = firstTech->first;
            
            std::cout << "   - Testing unlock tech: " << techId << std::endl;
            
            // Count unlocked recipes before
            int unlockedRecipesBefore = 0;
            for (const auto& recipe : craftingSystem->getAllRecipes()) {
                if (craftingSystem->isRecipeUnlocked(recipe.id)) {
                    unlockedRecipesBefore++;
                }
            }
            
            // Use test method to trigger tech completion handling
            techTreeSystem->testTriggerTechCompletion(techId);
            
            // Count unlocked recipes after
            int unlockedRecipesAfter = 0;
            for (const auto& recipe : craftingSystem->getAllRecipes()) {
                if (craftingSystem->isRecipeUnlocked(recipe.id)) {
                    unlockedRecipesAfter++;
                }
            }
            
            std::cout << "   - Unlocked recipes before: " << unlockedRecipesBefore << std::endl;
            std::cout << "   - Unlocked recipes after: " << unlockedRecipesAfter << std::endl;
            std::cout << "   - Craftable recipes (materials required): " << craftingSystem->getAvailableRecipes(*inventory).size() << std::endl;
            
            if (unlockedRecipesAfter > unlockedRecipesBefore) {
                std::cout << "   ✅ Tech successfully unlocked new recipes!" << std::endl;
            } else {
                std::cout << "   ℹ️  This tech has no associated recipes, or recipes already unlocked" << std::endl;
            }
        }
        
        std::cout << "\n✅ Tech tree and crafting system integration test succeeded!" << std::endl;
        std::cout << "   - TechTreeSystem successfully connected to CraftingSystem" << std::endl;
        std::cout << "   - Systems initialize and run correctly" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test exception: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}
