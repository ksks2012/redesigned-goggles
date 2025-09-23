#include "Systems/GameDataValidator.h"
#include <fstream>
#include <iostream>
#include <stack>
#include <algorithm>

GameDataValidator::GameDataValidator() {
}

bool GameDataValidator::loadDataFiles(const std::string& materialsPath,
                                     const std::string& recipesPath,
                                     const std::string& techTreePath) {
    try {
        // Load materials.json
        std::ifstream materialsFile(materialsPath);
        if (!materialsFile.is_open()) {
            std::cerr << "Failed to open materials file: " << materialsPath << std::endl;
            return false;
        }
        materialsFile >> materialsData;
        materialsFile.close();
        
        // Load recipes.json
        std::ifstream recipesFile(recipesPath);
        if (!recipesFile.is_open()) {
            std::cerr << "Failed to open recipes file: " << recipesPath << std::endl;
            return false;
        }
        recipesFile >> recipesData;
        recipesFile.close();
        
        // Load tech_tree.json
        std::ifstream techTreeFile(techTreePath);
        if (!techTreeFile.is_open()) {
            std::cerr << "Failed to open tech tree file: " << techTreePath << std::endl;
            return false;
        }
        techTreeFile >> techTreeData;
        techTreeFile.close();
        
        // Initialize cached data
        initializeCachedData();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading data files: " << e.what() << std::endl;
        return false;
    }
}

void GameDataValidator::initializeCachedData() {
    // Cache valid materials
    if (materialsData.contains("materials") && materialsData["materials"].is_array()) {
        for (const auto& material : materialsData["materials"]) {
            if (material.contains("name") && material["name"].is_string()) {
                validMaterials.insert(material["name"].get<std::string>());
            }
        }
    }
    
    // Cache valid recipes
    if (recipesData.contains("recipes") && recipesData["recipes"].is_array()) {
        for (const auto& recipe : recipesData["recipes"]) {
            if (recipe.contains("id") && recipe["id"].is_string()) {
                validRecipes.insert(recipe["id"].get<std::string>());
            }
        }
    }
    
    // Cache valid technologies and dependencies
    if (techTreeData.contains("tech_tree") && techTreeData["tech_tree"].contains("technologies") && 
        techTreeData["tech_tree"]["technologies"].is_array()) {
        for (const auto& tech : techTreeData["tech_tree"]["technologies"]) {
            if (tech.contains("id") && tech["id"].is_string()) {
                std::string techId = tech["id"].get<std::string>();
                validTechnologies.insert(techId);
                
                // Cache dependencies
                if (tech.contains("prerequisites") && tech["prerequisites"].is_array()) {
                    std::vector<std::string> deps;
                    for (const auto& prereq : tech["prerequisites"]) {
                        if (prereq.contains("tech_id") && prereq["tech_id"].is_string()) {
                            deps.push_back(prereq["tech_id"].get<std::string>());
                        }
                    }
                    techDependencies[techId] = deps;
                }
            }
        }
    }
}

ValidationResult GameDataValidator::validateAll() {
    ValidationResult result;
    
    // Validate individual components
    auto materialResult = validateMaterials();
    auto recipeResult = validateRecipes();
    auto techTreeResult = validateTechTree();
    auto crossRefResult = validateCrossReferences();
    
    // Merge results
    result.isValid = materialResult.isValid && recipeResult.isValid && 
                     techTreeResult.isValid && crossRefResult.isValid;
    
    result.errors.insert(result.errors.end(), materialResult.errors.begin(), materialResult.errors.end());
    result.errors.insert(result.errors.end(), recipeResult.errors.begin(), recipeResult.errors.end());
    result.errors.insert(result.errors.end(), techTreeResult.errors.begin(), techTreeResult.errors.end());
    result.errors.insert(result.errors.end(), crossRefResult.errors.begin(), crossRefResult.errors.end());
    
    result.warnings.insert(result.warnings.end(), materialResult.warnings.begin(), materialResult.warnings.end());
    result.warnings.insert(result.warnings.end(), recipeResult.warnings.begin(), recipeResult.warnings.end());
    result.warnings.insert(result.warnings.end(), techTreeResult.warnings.begin(), techTreeResult.warnings.end());
    result.warnings.insert(result.warnings.end(), crossRefResult.warnings.begin(), crossRefResult.warnings.end());
    
    return result;
}

ValidationResult GameDataValidator::validateMaterials() {
    ValidationResult result;
    
    if (!materialsData.contains("materials")) {
        result.addError("Materials data missing 'materials' array");
        return result;
    }
    
    if (!materialsData["materials"].is_array()) {
        result.addError("Materials 'materials' field is not an array");
        return result;
    }
    
    for (const auto& material : materialsData["materials"]) {
        auto materialResult = validateMaterialNode(material, 
                                                  material.contains("name") ? 
                                                  material["name"].get<std::string>() : "unnamed");
        
        result.errors.insert(result.errors.end(), materialResult.errors.begin(), materialResult.errors.end());
        result.warnings.insert(result.warnings.end(), materialResult.warnings.begin(), materialResult.warnings.end());
        
        if (!materialResult.isValid) {
            result.isValid = false;
        }
    }
    
    return result;
}

ValidationResult GameDataValidator::validateRecipes() {
    ValidationResult result;
    
    if (!recipesData.contains("recipes")) {
        result.addError("Recipes data missing 'recipes' array");
        return result;
    }
    
    if (!recipesData["recipes"].is_array()) {
        result.addError("Recipes 'recipes' field is not an array");
        return result;
    }
    
    for (const auto& recipe : recipesData["recipes"]) {
        auto recipeResult = validateRecipeNode(recipe, 
                                             recipe.contains("id") ? 
                                             recipe["id"].get<std::string>() : "unnamed");
        
        result.errors.insert(result.errors.end(), recipeResult.errors.begin(), recipeResult.errors.end());
        result.warnings.insert(result.warnings.end(), recipeResult.warnings.begin(), recipeResult.warnings.end());
        
        if (!recipeResult.isValid) {
            result.isValid = false;
        }
    }
    
    return result;
}

ValidationResult GameDataValidator::validateTechTree() {
    ValidationResult result;
    
    if (!techTreeData.contains("tech_tree")) {
        result.addError("Tech tree data missing 'tech_tree' object");
        return result;
    }
    
    const auto& techTree = techTreeData["tech_tree"];
    
    if (!techTree.contains("technologies") || !techTree["technologies"].is_array()) {
        result.addError("Tech tree missing 'technologies' array");
        return result;
    }
    
    // Validate each technology
    for (const auto& tech : techTree["technologies"]) {
        auto techResult = validateTechNode(tech, 
                                         tech.contains("id") ? 
                                         tech["id"].get<std::string>() : "unnamed");
        
        result.errors.insert(result.errors.end(), techResult.errors.begin(), techResult.errors.end());
        result.warnings.insert(result.warnings.end(), techResult.warnings.begin(), techResult.warnings.end());
        
        if (!techResult.isValid) {
            result.isValid = false;
        }
    }
    
    // Check for circular dependencies
    auto circularResult = checkCircularDependencies();
    result.errors.insert(result.errors.end(), circularResult.errors.begin(), circularResult.errors.end());
    result.warnings.insert(result.warnings.end(), circularResult.warnings.begin(), circularResult.warnings.end());
    
    if (!circularResult.isValid) {
        result.isValid = false;
    }
    
    return result;
}

ValidationResult GameDataValidator::validateCrossReferences() {
    ValidationResult result;
    
    // Validate recipe ingredients reference valid materials
    if (recipesData.contains("recipes") && recipesData["recipes"].is_array()) {
        for (const auto& recipe : recipesData["recipes"]) {
            std::string recipeId = recipe.contains("id") ? recipe["id"].get<std::string>() : "unnamed";
            
            if (recipe.contains("ingredients") && recipe["ingredients"].is_array()) {
                for (const auto& ingredient : recipe["ingredients"]) {
                    if (ingredient.contains("material") && ingredient["material"].is_string()) {
                        std::string materialName = ingredient["material"].get<std::string>();
                        if (!isMaterialValid(materialName)) {
                            result.addError("Recipe '" + recipeId + "' references invalid material: " + materialName);
                        }
                    }
                }
            }
            
            // Validate result material
            if (recipe.contains("result_material") && recipe["result_material"].is_string()) {
                std::string resultMaterial = recipe["result_material"].get<std::string>();
                if (!isMaterialValid(resultMaterial)) {
                    result.addWarning("Recipe '" + recipeId + "' result material not found in materials: " + resultMaterial);
                }
            }
        }
    }
    
    // Validate tech tree rewards reference valid recipes
    if (techTreeData.contains("tech_tree") && techTreeData["tech_tree"].contains("technologies")) {
        for (const auto& tech : techTreeData["tech_tree"]["technologies"]) {
            std::string techId = tech.contains("id") ? tech["id"].get<std::string>() : "unnamed";
            
            if (tech.contains("rewards") && tech["rewards"].is_array()) {
                for (const auto& reward : tech["rewards"]) {
                    if (reward.contains("type") && reward.contains("identifier")) {
                        std::string rewardType = reward["type"].get<std::string>();
                        std::string identifier = reward["identifier"].get<std::string>();
                        
                        if (rewardType == "recipe") {
                            if (!isRecipeValid(identifier)) {
                                result.addError("Technology '" + techId + "' rewards invalid recipe: " + identifier);
                            }
                        }
                        // Add more validation for other reward types if needed
                    }
                }
            }
            
            // Validate prerequisites
            if (tech.contains("prerequisites") && tech["prerequisites"].is_array()) {
                for (const auto& prereq : tech["prerequisites"]) {
                    if (prereq.contains("tech_id") && prereq["tech_id"].is_string()) {
                        std::string prereqTechId = prereq["tech_id"].get<std::string>();
                        if (!isTechnologyValid(prereqTechId)) {
                            result.addError("Technology '" + techId + "' has invalid prerequisite: " + prereqTechId);
                        }
                    }
                }
            }
            
            // Validate unlocks
            if (tech.contains("unlocks") && tech["unlocks"].is_array()) {
                for (const auto& unlock : tech["unlocks"]) {
                    if (unlock.is_string()) {
                        std::string unlockTechId = unlock.get<std::string>();
                        if (!isTechnologyValid(unlockTechId)) {
                            result.addError("Technology '" + techId + "' unlocks invalid technology: " + unlockTechId);
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

ValidationResult GameDataValidator::checkCircularDependencies() {
    ValidationResult result;
    
    // Use DFS to detect cycles
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> inStack;
    
    for (const auto& techId : validTechnologies) {
        if (visited.find(techId) == visited.end()) {
            std::function<bool(const std::string&)> hasCycle = [&](const std::string& current) -> bool {
                visited.insert(current);
                inStack.insert(current);
                
                if (techDependencies.find(current) != techDependencies.end()) {
                    for (const auto& dep : techDependencies[current]) {
                        if (inStack.find(dep) != inStack.end()) {
                            return true; // Cycle detected
                        }
                        if (visited.find(dep) == visited.end() && hasCycle(dep)) {
                            return true;
                        }
                    }
                }
                
                inStack.erase(current);
                return false;
            };
            
            if (hasCycle(techId)) {
                result.addError("Circular dependency detected in tech tree involving: " + techId);
            }
        }
    }
    
    return result;
}

ValidationResult GameDataValidator::validateMaterialNode(const nlohmann::json& material, const std::string& materialName) {
    ValidationResult result;
    
    // Required fields
    std::vector<std::string> requiredFields = {"name", "type", "rarity", "base_quantity"};
    
    for (const auto& field : requiredFields) {
        if (!material.contains(field)) {
            result.addError("Material '" + materialName + "' missing required field: " + field);
        }
    }
    
    // Validate field types
    if (material.contains("rarity") && !material["rarity"].is_number_integer()) {
        result.addError("Material '" + materialName + "' rarity must be an integer");
    }
    
    if (material.contains("base_quantity") && !material["base_quantity"].is_number_integer()) {
        result.addError("Material '" + materialName + "' base_quantity must be an integer");
    }
    
    return result;
}

ValidationResult GameDataValidator::validateRecipeNode(const nlohmann::json& recipe, const std::string& recipeId) {
    ValidationResult result;
    
    // Required fields
    std::vector<std::string> requiredFields = {"id", "name", "description", "ingredients", "result_material", "success_rate"};
    
    for (const auto& field : requiredFields) {
        if (!recipe.contains(field)) {
            result.addError("Recipe '" + recipeId + "' missing required field: " + field);
        }
    }
    
    // Validate ingredients
    if (recipe.contains("ingredients") && recipe["ingredients"].is_array()) {
        for (const auto& ingredient : recipe["ingredients"]) {
            if (!ingredient.contains("material") || !ingredient.contains("quantity")) {
                result.addError("Recipe '" + recipeId + "' ingredient missing material or quantity");
            }
            if (ingredient.contains("quantity") && !ingredient["quantity"].is_number_integer()) {
                result.addError("Recipe '" + recipeId + "' ingredient quantity must be an integer");
            }
        }
    } else if (recipe.contains("ingredients")) {
        result.addError("Recipe '" + recipeId + "' ingredients must be an array");
    }
    
    // Validate success rate
    if (recipe.contains("success_rate")) {
        if (recipe["success_rate"].is_number()) {
            double successRate = recipe["success_rate"].get<double>();
            if (successRate < 0.0 || successRate > 1.0) {
                result.addError("Recipe '" + recipeId + "' success_rate must be between 0.0 and 1.0");
            }
        } else {
            result.addError("Recipe '" + recipeId + "' success_rate must be a number");
        }
    }
    
    return result;
}

ValidationResult GameDataValidator::validateTechNode(const nlohmann::json& tech, const std::string& techId) {
    ValidationResult result;
    
    // Get validation rules from tech tree data
    std::vector<std::string> validTypes = {"SURVIVAL", "CRAFTING", "AGRICULTURE", "BUILDING", "MILITARY", "ADVANCED"};
    std::vector<std::string> validStatuses = {"LOCKED", "AVAILABLE", "RESEARCHING", "COMPLETED"};
    
    // Required fields
    std::vector<std::string> requiredFields = {"id", "name", "description", "type", "research_cost", "position", "prerequisites", "rewards", "unlocks", "initial_status"};
    
    for (const auto& field : requiredFields) {
        if (!tech.contains(field)) {
            result.addError("Technology '" + techId + "' missing required field: " + field);
        }
    }
    
    // Validate type
    if (tech.contains("type") && tech["type"].is_string()) {
        std::string type = tech["type"].get<std::string>();
        if (std::find(validTypes.begin(), validTypes.end(), type) == validTypes.end()) {
            result.addError("Technology '" + techId + "' has invalid type: " + type);
        }
    }
    
    // Validate initial status
    if (tech.contains("initial_status") && tech["initial_status"].is_string()) {
        std::string status = tech["initial_status"].get<std::string>();
        if (std::find(validStatuses.begin(), validStatuses.end(), status) == validStatuses.end()) {
            result.addError("Technology '" + techId + "' has invalid initial_status: " + status);
        }
    }
    
    // Validate research cost
    if (tech.contains("research_cost") && tech["research_cost"].is_number_integer()) {
        int cost = tech["research_cost"].get<int>();
        if (cost < 10 || cost > 1000) {
            result.addWarning("Technology '" + techId + "' research cost (" + std::to_string(cost) + ") outside recommended range (10-1000)");
        }
    }
    
    // Validate position
    if (tech.contains("position") && tech["position"].is_object()) {
        const auto& pos = tech["position"];
        if (!pos.contains("x") || !pos.contains("y")) {
            result.addError("Technology '" + techId + "' position missing x or y coordinate");
        }
    }
    
    return result;
}

bool GameDataValidator::isMaterialValid(const std::string& materialName) const {
    return validMaterials.find(materialName) != validMaterials.end();
}

bool GameDataValidator::isRecipeValid(const std::string& recipeId) const {
    return validRecipes.find(recipeId) != validRecipes.end();
}

bool GameDataValidator::isTechnologyValid(const std::string& techId) const {
    return validTechnologies.find(techId) != validTechnologies.end();
}

std::string GameDataValidator::getValidationReport(const ValidationResult& result) const {
    std::string report;
    
    if (result.isValid) {
        report += "✅ Validation PASSED\n";
    } else {
        report += "❌ Validation FAILED\n";
    }
    
    if (!result.errors.empty()) {
        report += "\n🔴 ERRORS:\n";
        for (const auto& error : result.errors) {
            report += "  - " + error + "\n";
        }
    }
    
    if (!result.warnings.empty()) {
        report += "\n🟡 WARNINGS:\n";
        for (const auto& warning : result.warnings) {
            report += "  - " + warning + "\n";
        }
    }
    
    if (result.errors.empty() && result.warnings.empty()) {
        report += "\n✨ All data is valid!\n";
    }
    
    return report;
}
