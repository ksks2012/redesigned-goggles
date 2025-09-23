#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "nlohmann/json.hpp"

/**
 * @brief Validation result structure
 */
struct ValidationResult {
    bool isValid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    void addError(const std::string& message) {
        errors.push_back(message);
        isValid = false;
    }
    
    void addWarning(const std::string& message) {
        warnings.push_back(message);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

/**
 * @brief Game data validator for materials, recipes, and tech tree
 */
class GameDataValidator {
private:
    nlohmann::json materialsData;
    nlohmann::json recipesData;
    nlohmann::json techTreeData;
    
    // Cached data for validation
    std::unordered_set<std::string> validMaterials;
    std::unordered_set<std::string> validRecipes;
    std::unordered_set<std::string> validTechnologies;
    std::unordered_map<std::string, std::vector<std::string>> techDependencies;
    
public:
    /**
     * @brief Constructor
     */
    GameDataValidator();
    
    /**
     * @brief Load data files for validation
     * @param materialsPath Path to materials.json
     * @param recipesPath Path to recipes.json
     * @param techTreePath Path to tech_tree.json
     * @return Whether all files loaded successfully
     */
    bool loadDataFiles(const std::string& materialsPath,
                       const std::string& recipesPath,
                       const std::string& techTreePath);
    
    /**
     * @brief Validate all game data
     * @return Validation result
     */
    ValidationResult validateAll();
    
    /**
     * @brief Validate materials data
     * @return Validation result
     */
    ValidationResult validateMaterials();
    
    /**
     * @brief Validate recipes data
     * @return Validation result
     */
    ValidationResult validateRecipes();
    
    /**
     * @brief Validate tech tree data
     * @return Validation result
     */
    ValidationResult validateTechTree();
    
    /**
     * @brief Validate cross-references between systems
     * @return Validation result
     */
    ValidationResult validateCrossReferences();
    
    /**
     * @brief Check if a material exists
     * @param materialName Material name to check
     * @return Whether material exists
     */
    bool isMaterialValid(const std::string& materialName) const;
    
    /**
     * @brief Check if a recipe exists
     * @param recipeId Recipe ID to check
     * @return Whether recipe exists
     */
    bool isRecipeValid(const std::string& recipeId) const;
    
    /**
     * @brief Check if a technology exists
     * @param techId Technology ID to check
     * @return Whether technology exists
     */
    bool isTechnologyValid(const std::string& techId) const;
    
    /**
     * @brief Get validation report as formatted string
     * @param result Validation result
     * @return Formatted report string
     */
    std::string getValidationReport(const ValidationResult& result) const;

private:
    /**
     * @brief Initialize cached data from loaded JSON
     */
    void initializeCachedData();
    
    /**
     * @brief Validate JSON structure against schema
     * @param data JSON data to validate
     * @param schema Expected schema structure
     * @param context Context name for error messages
     * @return Validation result
     */
    ValidationResult validateJsonStructure(const nlohmann::json& data,
                                          const nlohmann::json& schema,
                                          const std::string& context);
    
    /**
     * @brief Check for circular dependencies in tech tree
     * @return Validation result
     */
    ValidationResult checkCircularDependencies();
    
    /**
     * @brief Validate tech tree node
     * @param tech Technology node to validate
     * @param techId Technology ID
     * @return Validation result
     */
    ValidationResult validateTechNode(const nlohmann::json& tech, const std::string& techId);
    
    /**
     * @brief Validate recipe node
     * @param recipe Recipe node to validate
     * @param recipeId Recipe ID
     * @return Validation result
     */
    ValidationResult validateRecipeNode(const nlohmann::json& recipe, const std::string& recipeId);
    
    /**
     * @brief Validate material node
     * @param material Material node to validate
     * @param materialName Material name
     * @return Validation result
     */
    ValidationResult validateMaterialNode(const nlohmann::json& material, const std::string& materialName);
};
