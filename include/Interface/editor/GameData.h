#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "Core/Card.h"
#include "Systems/CraftingSystem.h"
#include <vector>
#include <map>
#include <string>
#include <functional>

/**
 * Data structures for the game editor system
 */

// Material/Card template for editing
struct MaterialTemplate {
    std::string id;
    std::string name;
    CardType type;
    int rarity;
    std::map<AttributeType, float> attributes;
    std::string description;
    
    // Convert to actual Card
    Card toCard(int quantity = 1) const;
    
    // Create from existing Card
    static MaterialTemplate fromCard(const Card& card);
};

// Event condition types
enum class ConditionType {
    LOCATION,           // Player at specific location
    TIME,              // Game time/turn
    INVENTORY_HAS,     // Has specific item in inventory
    INVENTORY_COUNT,   // Has X amount of specific item
    PLAYER_HEALTH,     // Player health condition
    RANDOM_CHANCE      // Random probability
};

// Event effect types
enum class EffectType {
    DAMAGE_PLAYER,     // Deal damage to player
    HEAL_PLAYER,       // Heal player
    ADD_ITEM,          // Add item to inventory
    REMOVE_ITEM,       // Remove item from inventory
    SHOW_MESSAGE,      // Display message to player
    TRIGGER_EVENT      // Trigger another event
};

// Event condition
struct EventCondition {
    ConditionType type;
    std::string target;    // Item ID, location name, etc.
    float value;           // Threshold, amount, percentage
    std::string operator_; // ">=", "==", "<", etc.
    
    bool evaluate() const; // Evaluate if condition is met
};

// Event effect
struct EventEffect {
    EffectType type;
    std::string target;    // Item ID, message text, etc.
    float value;           // Amount, damage, etc.
    
    void execute() const;  // Execute the effect
};

// Game event template
struct EventTemplate {
    std::string id;
    std::string name;
    std::string description;
    std::vector<EventCondition> conditions;
    std::vector<EventEffect> effects;
    bool isRepeatable;
    int priority;
    
    bool canTrigger() const; // Check if all conditions are met
    void trigger() const;    // Execute all effects
};

/**
 * Central data manager for the editor system
 * Handles all game data that can be edited
 */
class Game; // Forward declaration

class GameDataManager {
public:
    GameDataManager();
    explicit GameDataManager(Game* gameInstance);
    ~GameDataManager() = default;

    // Game instance management
    void setGameInstance(Game* gameInstance) { gameInstance_ = gameInstance; }
    Game* getGameInstance() const { return gameInstance_; }
    
    // Live data access
    void syncFromGame(); // Sync editor data from current game state
    void syncToGame();   // Apply editor changes to game
    
    // Real-time game state inspection
    struct GameState {
        std::vector<Card> inventoryCards;
        int playerHealth;
        std::vector<std::string> availableRecipes;
        // Add more state fields as needed
    };
    
    GameState getCurrentGameState() const;

    // Material management
    void addMaterial(const MaterialTemplate& material);
    void removeMaterial(const std::string& id);
    void updateMaterial(const std::string& id, const MaterialTemplate& material);
    MaterialTemplate* getMaterial(const std::string& id);
    const std::vector<MaterialTemplate>& getAllMaterials() const { return materials_; }
    
    // Recipe management
    void addRecipe(const Recipe& recipe);
    void removeRecipe(const std::string& name);
    void updateRecipe(const std::string& name, const Recipe& recipe);
    Recipe* getRecipe(const std::string& name);
    const std::vector<Recipe>& getAllRecipes() const { return recipes_; }
    
    // Event management
    void addEvent(const EventTemplate& event);
    void removeEvent(const std::string& id);
    void updateEvent(const std::string& id, const EventTemplate& event);
    EventTemplate* getEvent(const std::string& id);
    const std::vector<EventTemplate>& getAllEvents() const { return events_; }
    
    // Data validation
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ValidationResult validateData() const;
    
    // File operations
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    
    // Change tracking for undo/redo
    void pushState();
    bool undo();
    bool redo();
    
    // Change notification system
    using ChangeCallback = std::function<void(const std::string& type, const std::string& id)>;
    void setChangeCallback(ChangeCallback callback) { changeCallback_ = callback; }

private:
    std::vector<MaterialTemplate> materials_;
    std::vector<Recipe> recipes_;
    std::vector<EventTemplate> events_;
    
    // Game instance reference for live data
    Game* gameInstance_;
    
    // Undo/redo system
    struct DataState {
        std::vector<MaterialTemplate> materials;
        std::vector<Recipe> recipes;
        std::vector<EventTemplate> events;
    };
    
    std::vector<DataState> undoStack_;
    std::vector<DataState> redoStack_;
    static const size_t MAX_UNDO_STATES = 50;
    
    ChangeCallback changeCallback_;
    
    // Helper functions
    void notifyChange(const std::string& type, const std::string& id);
    DataState captureCurrentState() const;
    void restoreState(const DataState& state);
    void initializeDefaults(); // Initialize with default data
    
    // Validation helpers
    bool validateMaterialReferences() const;
    bool validateRecipeReferences() const;
    bool validateEventReferences() const;
};

#endif // GAMEDATA_H
