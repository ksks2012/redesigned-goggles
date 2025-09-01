#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Card.h"
#include "Event.h"

// Forward declarations
struct Recipe;
class Inventory;
class CraftingSystem;
class Controller;

/**
 * Version management and validation system for game data
 * Ensures consistency between materials, recipes, and events
 */
namespace DataManagement {

    /**
     * Version information for data files
     */
    struct Version {
        int major = 1;
        int minor = 0;
        int patch = 0;
        
        Version() = default;
        Version(int maj, int min, int pat) : major(maj), minor(min), patch(pat) {}
        
        // Parse version string "1.0.0"
        static Version fromString(const std::string& versionStr);
        
        // Convert to string "1.0.0"
        std::string toString() const;
        
        // Version comparison
        bool operator==(const Version& other) const;
        bool operator!=(const Version& other) const;
        bool isCompatible(const Version& other) const; // Major version compatibility
    };

    /**
     * Material data structure for JSON serialization
     */
    struct MaterialData {
        std::string name;
        int rarity;
        CardType type;
        int baseQuantity = 1;
        std::unordered_map<AttributeType, float> attributes;
        
        // Convert to/from Card
        Card toCard() const;
        static MaterialData fromCard(const Card& card);
    };

    /**
     * Recipe data structure for JSON serialization
     */
    struct RecipeData {
        std::string id;
        std::string name;
        std::string description;
        std::vector<std::pair<std::string, int>> ingredients; // material name, quantity
        std::string resultMaterial;
        float successRate;
        int unlockLevel;
        bool isUnlocked;
    };

    /**
     * Event data structure for JSON serialization
     */
    struct EventData {
        std::string name;
        std::string description;
        std::vector<std::string> rewardMaterials;
        std::vector<std::string> penaltyMaterials;
        float probability;
    };

    /**
     * Game configuration with global version
     */
    struct GameConfig {
        Version version;
        std::string configName;
        std::string description;
        std::unordered_map<std::string, std::string> settings;
    };

    /**
     * Data validation results
     */
    struct ValidationResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        void addError(const std::string& error);
        void addWarning(const std::string& warning);
        bool hasErrors() const { return !errors.empty(); }
        bool hasWarnings() const { return !warnings.empty(); }
        std::string getSummary() const;
    };

    /**
     * Centralized data manager for loading, validating, and managing game data
     */
    class GameDataManager {
    public:
        GameDataManager();
        ~GameDataManager();

        // Loading functions
        bool loadGameConfig(const std::string& configPath = "data/game_config.json");
        bool loadMaterials(const std::string& materialsPath = "data/materials.json");
        bool loadRecipes(const std::string& recipesPath = "data/recipes.json");
        bool loadEvents(const std::string& eventsPath = "data/events.json");
        
        // Load all data files
        bool loadAllData(const std::string& dataDirectory = "data/");
        
        // Saving functions
        bool saveGameConfig(const std::string& configPath = "data/game_config.json") const;
        bool saveMaterials(const std::string& materialsPath = "data/materials.json") const;
        bool saveRecipes(const std::string& recipesPath = "data/recipes.json") const;
        bool saveEvents(const std::string& eventsPath = "data/events.json") const;
        
        // Save all data files
        bool saveAllData(const std::string& dataDirectory = "data/") const;

        // Validation functions
        ValidationResult validateVersion() const;
        ValidationResult validateDataConsistency() const;
        ValidationResult validateMaterials() const;
        ValidationResult validateRecipes() const;
        ValidationResult validateEvents() const;
        
        // Comprehensive validation
        ValidationResult validateAll() const;

        // Data access
        const GameConfig& getGameConfig() const { return gameConfig; }
        const std::vector<MaterialData>& getMaterials() const { return materials; }
        const std::vector<RecipeData>& getRecipes() const { return recipes; }
        const std::vector<EventData>& getEvents() const { return events; }

        // Data modification
        void setGameConfig(const GameConfig& config) { gameConfig = config; }
        void setMaterials(const std::vector<MaterialData>& mats) { materials = mats; }
        void setRecipes(const std::vector<RecipeData>& recs) { recipes = recs; }
        void setEvents(const std::vector<EventData>& evts) { events = evts; }

        // Utility functions
        bool materialExists(const std::string& name, int rarity) const;
        MaterialData* findMaterial(const std::string& name, int rarity);
        const MaterialData* findMaterial(const std::string& name, int rarity) const;
        
        RecipeData* findRecipe(const std::string& id);
        const RecipeData* findRecipe(const std::string& id) const;
        
        // Apply data to game systems
        bool applyToInventory(Inventory& inventory) const;
        bool applyToCraftingSystem(CraftingSystem& craftingSystem) const;
        bool applyToController(Controller& controller) const;

        // Create default data files
        void createDefaultGameConfig();
        void createDefaultMaterials();
        void createDefaultRecipes();
        void createDefaultEvents();
        void createDefaultDataFiles(const std::string& dataDirectory = "data/");

    private:
        GameConfig gameConfig;
        std::vector<MaterialData> materials;
        std::vector<RecipeData> recipes;
        std::vector<EventData> events;

        // Version tracking for each file
        Version materialsVersion;
        Version recipesVersion;
        Version eventsVersion;

        // Helper functions for JSON parsing
        bool parseGameConfigJson(const std::string& jsonContent);
        bool parseMaterialsJson(const std::string& jsonContent);
        bool parseRecipesJson(const std::string& jsonContent);
        bool parseEventsJson(const std::string& jsonContent);

        // Helper functions for JSON generation
        std::string generateGameConfigJson() const;
        std::string generateMaterialsJson() const;
        std::string generateRecipesJson() const;
        std::string generateEventsJson() const;

        // File I/O helpers
        bool readFileContent(const std::string& filePath, std::string& content) const;
        bool writeFileContent(const std::string& filePath, const std::string& content) const;
        bool ensureDirectoryExists(const std::string& dirPath) const;
    };

} // namespace DataManagement
