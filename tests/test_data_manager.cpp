#include "../lib/catch2/catch.hpp"
#include "Systems/DataManager.h"
#include <filesystem>
#include <fstream>

using namespace DataManagement;

TEST_CASE("Version management functionality", "[DataManager][Version]") {
    SECTION("Version string parsing and conversion") {
        Version version = Version::fromString("1.2.3");
        REQUIRE(version.major == 1);
        REQUIRE(version.minor == 2);
        REQUIRE(version.patch == 3);
        
        REQUIRE(version.toString() == "1.2.3");
    }
    
    SECTION("Version comparison") {
        Version v1(1, 0, 0);
        Version v2(1, 0, 0);
        Version v3(1, 1, 0);
        Version v4(2, 0, 0);
        
        REQUIRE(v1 == v2);
        REQUIRE(v1 != v3);
        REQUIRE(v1 != v4);
        
        // Compatibility (same major version)
        REQUIRE(v1.isCompatible(v2));
        REQUIRE(v1.isCompatible(v3));
        REQUIRE(!v1.isCompatible(v4));
    }
}

TEST_CASE("MaterialData conversion", "[DataManager][MaterialData]") {
    SECTION("Card to MaterialData and back") {
        Card originalCard("Iron", 2, CardType::METAL, 5);
        originalCard.setAttribute(AttributeType::WEIGHT, 10.0f);
        originalCard.setAttribute(AttributeType::DURABILITY, 100.0f);
        
        MaterialData materialData = MaterialData::fromCard(originalCard);
        Card convertedCard = materialData.toCard();
        
        REQUIRE(convertedCard.name == originalCard.name);
        REQUIRE(convertedCard.rarity == originalCard.rarity);
        REQUIRE(convertedCard.type == originalCard.type);
        REQUIRE(convertedCard.quantity == originalCard.quantity);
        REQUIRE(convertedCard.getAttribute(AttributeType::WEIGHT) == 10.0f);
        REQUIRE(convertedCard.getAttribute(AttributeType::DURABILITY) == 100.0f);
    }
}

TEST_CASE("GameDataManager basic operations", "[DataManager][GameDataManager]") {
    GameDataManager manager;
    
    SECTION("Default data creation") {
        manager.createDefaultDataFiles("test_data/");
        
        // Check that default data was created
        const auto& materials = manager.getMaterials();
        const auto& recipes = manager.getRecipes();
        const auto& events = manager.getEvents();
        
        REQUIRE(materials.size() > 0);
        REQUIRE(recipes.size() > 0);
        REQUIRE(events.size() > 0);
        
        // Check some expected default materials
        bool foundWood = false, foundMetal = false;
        for (const auto& material : materials) {
            if (material.name == "Wood") foundWood = true;
            if (material.name == "Metal") foundMetal = true;
        }
        REQUIRE(foundWood);
        REQUIRE(foundMetal);
    }
    
    SECTION("Material lookup functionality") {
        manager.createDefaultMaterials();
        
        REQUIRE(manager.materialExists("Wood", 1));
        REQUIRE(manager.materialExists("Metal", 2));
        REQUIRE(!manager.materialExists("NonExistent", 1));
        
        const MaterialData* wood = manager.findMaterial("Wood", 1);
        REQUIRE(wood != nullptr);
        REQUIRE(wood->name == "Wood");
        REQUIRE(wood->rarity == 1);
        
        const MaterialData* nonExistent = manager.findMaterial("NonExistent", 1);
        REQUIRE(nonExistent == nullptr);
    }
    
    SECTION("Recipe lookup functionality") {
        manager.createDefaultRecipes();
        
        const RecipeData* medkit = manager.findRecipe("medkit");
        REQUIRE(medkit != nullptr);
        REQUIRE(medkit->name == "Medkit");
        REQUIRE(medkit->ingredients.size() > 0);
        
        const RecipeData* nonExistent = manager.findRecipe("nonexistent");
        REQUIRE(nonExistent == nullptr);
    }
}

TEST_CASE("Data validation", "[DataManager][Validation]") {
    GameDataManager manager;
    
    SECTION("Version validation") {
        manager.createDefaultDataFiles();
        
        ValidationResult result = manager.validateVersion();
        REQUIRE(result.isValid == true);
        REQUIRE(result.errors.empty());
    }
    
    SECTION("Material validation") {
        manager.createDefaultMaterials();
        
        ValidationResult result = manager.validateMaterials();
        REQUIRE(result.isValid == true);
        
        // Add duplicate material to test validation
        std::vector<MaterialData> materials = manager.getMaterials();
        materials.push_back(materials[0]); // Duplicate first material
        manager.setMaterials(materials);
        
        ValidationResult duplicateResult = manager.validateMaterials();
        REQUIRE(duplicateResult.isValid == false);
        REQUIRE(duplicateResult.errors.size() > 0);
    }
    
    SECTION("Recipe validation") {
        manager.createDefaultRecipes();
        
        ValidationResult result = manager.validateRecipes();
        REQUIRE(result.isValid == true);
        
        // Add recipe with duplicate ID
        std::vector<RecipeData> recipes = manager.getRecipes();
        recipes.push_back(recipes[0]); // Duplicate first recipe
        manager.setRecipes(recipes);
        
        ValidationResult duplicateResult = manager.validateRecipes();
        REQUIRE(duplicateResult.isValid == false);
        REQUIRE(duplicateResult.errors.size() > 0);
    }
    
    SECTION("Data consistency validation") {
        manager.createDefaultDataFiles();
        
        ValidationResult result = manager.validateDataConsistency();
        REQUIRE(result.isValid == true);
        
        // Create recipe with non-existent material
        std::vector<RecipeData> recipes = manager.getRecipes();
        RecipeData invalidRecipe;
        invalidRecipe.id = "invalid_recipe";
        invalidRecipe.name = "Invalid Recipe";
        invalidRecipe.description = "Recipe with non-existent material";
        invalidRecipe.ingredients = {{"NonExistentMaterial", 1}};
        invalidRecipe.resultMaterial = "AnotherNonExistentMaterial";
        invalidRecipe.successRate = 1.0f;
        recipes.push_back(invalidRecipe);
        manager.setRecipes(recipes);
        
        ValidationResult invalidResult = manager.validateDataConsistency();
        REQUIRE(invalidResult.isValid == false);
        REQUIRE(invalidResult.errors.size() >= 2); // Missing ingredient + missing result
    }
    
    SECTION("Comprehensive validation") {
        manager.createDefaultDataFiles();
        
        ValidationResult result = manager.validateAll();
        REQUIRE(result.isValid == true);
        
        // Test validation summary
        std::string summary = result.getSummary();
        REQUIRE(summary.find("PASSED") != std::string::npos);
    }
}

TEST_CASE("File I/O operations", "[DataManager][FileIO]") {
    GameDataManager manager;
    const std::string testDir = "test_data_temp/";
    
    SECTION("Save and load game config") {
        // Ensure test directory is clean
        std::filesystem::remove_all(testDir);
        
        manager.createDefaultGameConfig();
        
        bool saveSuccess = manager.saveGameConfig(testDir + "game_config.json");
        REQUIRE(saveSuccess == true);
        
        // Verify file exists
        REQUIRE(std::filesystem::exists(testDir + "game_config.json"));
        
        // Create new manager and load
        GameDataManager newManager;
        bool loadSuccess = newManager.loadGameConfig(testDir + "game_config.json");
        REQUIRE(loadSuccess == true);
        
        // Verify data matches
        REQUIRE(newManager.getGameConfig().configName == manager.getGameConfig().configName);
        REQUIRE(newManager.getGameConfig().version.toString() == manager.getGameConfig().version.toString());
        
        // Cleanup
        std::filesystem::remove_all(testDir);
    }
    
    SECTION("Save and load all data") {
        // Ensure test directory is clean
        std::filesystem::remove_all(testDir);
        
        manager.createDefaultDataFiles();
        
        bool saveSuccess = manager.saveAllData(testDir);
        REQUIRE(saveSuccess == true);
        
        // Verify all files exist
        REQUIRE(std::filesystem::exists(testDir + "game_config.json"));
        REQUIRE(std::filesystem::exists(testDir + "materials.json"));
        REQUIRE(std::filesystem::exists(testDir + "recipes.json"));
        REQUIRE(std::filesystem::exists(testDir + "events.json"));
        
        // Create new manager and load all data
        GameDataManager newManager;
        bool loadSuccess = newManager.loadAllData(testDir);
        REQUIRE(loadSuccess == true);
        
        // Verify data counts match
        REQUIRE(newManager.getMaterials().size() == manager.getMaterials().size());
        REQUIRE(newManager.getRecipes().size() == manager.getRecipes().size());
        REQUIRE(newManager.getEvents().size() == manager.getEvents().size());
        
        // Cleanup
        std::filesystem::remove_all(testDir);
    }
}

TEST_CASE("ValidationResult functionality", "[DataManager][ValidationResult]") {
    SECTION("Error and warning handling") {
        ValidationResult result;
        
        REQUIRE(result.isValid == true);
        REQUIRE(!result.hasErrors());
        REQUIRE(!result.hasWarnings());
        
        result.addWarning("Test warning");
        REQUIRE(result.isValid == true); // Warnings don't make it invalid
        REQUIRE(result.hasWarnings());
        
        result.addError("Test error");
        REQUIRE(result.isValid == false); // Errors make it invalid
        REQUIRE(result.hasErrors());
        
        std::string summary = result.getSummary();
        REQUIRE(summary.find("FAILED") != std::string::npos);
        REQUIRE(summary.find("Test warning") != std::string::npos);
        REQUIRE(summary.find("Test error") != std::string::npos);
    }
}
