/**
 * @file game_data_validation_tool.cpp
 * @brief Game data validation tool to check consistency of materials, recipes, and tech tree
 */

#include "Systems/GameDataValidator.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== Game Data Validation Tool ===" << std::endl;
    
    // Set default paths
    std::string materialsPath = "data/materials.json";
    std::string recipesPath = "data/recipes.json";  
    std::string techTreePath = "data/tech_tree.json";
    
    // Allow command line arguments to override paths
    if (argc >= 4) {
        materialsPath = argv[1];
        recipesPath = argv[2];
        techTreePath = argv[3];
    }
    
    std::cout << "Checking files:" << std::endl;
    std::cout << "  Materials file: " << materialsPath << std::endl;
    std::cout << "  Recipes file: " << recipesPath << std::endl;
    std::cout << "  Tech tree file: " << techTreePath << std::endl;
    std::cout << std::endl;
    
    // Create validator
    GameDataValidator validator;
    
    // Load data files
    std::cout << "📂 Loading data files..." << std::endl;
    if (!validator.loadDataFiles(materialsPath, recipesPath, techTreePath)) {
        std::cerr << "❌ Unable to load data files, please check file paths and format" << std::endl;
        return -1;
    }
    std::cout << "✅ Data files loaded successfully" << std::endl;
    
    // Execute full validation
    std::cout << "\n🔍 Starting game data validation..." << std::endl;
    auto result = validator.validateAll();
    
    // Output results
    std::cout << validator.getValidationReport(result) << std::endl;
    
    // Detailed validation report
    std::cout << "\n📊 Detailed validation results:" << std::endl;
    
    // Validate materials
    auto materialResult = validator.validateMaterials();
    std::cout << "  Materials validation: " << (materialResult.isValid ? "✅ Passed" : "❌ Failed") << std::endl;
    if (materialResult.hasErrors()) {
        std::cout << "    Error count: " << materialResult.errors.size() << std::endl;
    }
    if (materialResult.hasWarnings()) {
        std::cout << "    Warning count: " << materialResult.warnings.size() << std::endl;
    }
    
    // Validate recipes
    auto recipeResult = validator.validateRecipes();
    std::cout << "  Recipes validation: " << (recipeResult.isValid ? "✅ Passed" : "❌ Failed") << std::endl;
    if (recipeResult.hasErrors()) {
        std::cout << "    Error count: " << recipeResult.errors.size() << std::endl;
    }
    if (recipeResult.hasWarnings()) {
        std::cout << "    Warning count: " << recipeResult.warnings.size() << std::endl;
    }
    
    // Validate tech tree
    auto techTreeResult = validator.validateTechTree();
    std::cout << "  Tech tree validation: " << (techTreeResult.isValid ? "✅ Passed" : "❌ Failed") << std::endl;
    if (techTreeResult.hasErrors()) {
        std::cout << "    Error count: " << techTreeResult.errors.size() << std::endl;
    }
    if (techTreeResult.hasWarnings()) {
        std::cout << "    Warning count: " << techTreeResult.warnings.size() << std::endl;
    }
    
    // Validate cross-references
    auto crossRefResult = validator.validateCrossReferences();
    std::cout << "  Cross-reference validation: " << (crossRefResult.isValid ? "✅ Passed" : "❌ Failed") << std::endl;
    if (crossRefResult.hasErrors()) {
        std::cout << "    Error count: " << crossRefResult.errors.size() << std::endl;
    }
    if (crossRefResult.hasWarnings()) {
        std::cout << "    Warning count: " << crossRefResult.warnings.size() << std::endl;
    }
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    
    if (result.isValid) {
        std::cout << "🎉 All game data validation passed! Safe to use." << std::endl;
        return 0;
    } else {
        std::cout << "⚠️  Game data has issues, please fix them before using." << std::endl;
        return 1;
    }
}
