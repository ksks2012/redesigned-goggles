#pragma once
#include <string>
#include <nlohmann/json.hpp>

/**
 * Building types supported in the base building system
 * Each type has different properties and gameplay effects
 */
enum class BuildingType {
    NONE = 0,
    WALL,       // Defense structure: +10 defense, 100 durability
    FARM,       // Food production: +0 defense, 80 durability, produces food over time
    WORKSHOP,   // Crafting enhancement: +0 defense, 60 durability, unlocks advanced recipes
    STORAGE,    // Inventory expansion: +0 defense, 40 durability, increases inventory capacity
    WATCHTOWER  // Early warning: +5 defense, 120 durability, detects incoming events
};

/**
 * Individual building instance placed on the base grid
 * Represents a constructed building with current state and properties
 */
class Building {
public:
    Building(const std::string& name, BuildingType type, int x, int y, 
             int maxDurability = 100, int defenseValue = 0);
    ~Building() = default;

    // Position and identification
    int getX() const { return x_; }
    int getY() const { return y_; }
    const std::string& getName() const { return name_; }
    BuildingType getType() const { return type_; }

    // Durability management
    int getDurability() const { return durability_; }
    int getMaxDurability() const { return maxDurability_; }
    void takeDamage(int damage);
    void repair(int amount);
    bool isDestroyed() const { return durability_ <= 0; }
    float getDurabilityPercentage() const;

    // Defense and functionality
    int getDefenseValue() const { return isDestroyed() ? 0 : defenseValue_; }
    
    // Building state for UI rendering
    bool needsRepair() const { return durability_ < maxDurability_ * 0.3f; }
    bool isOperational() const { return durability_ > 0; }

    // Serialization for save/load
    nlohmann::json toJson() const;
    static Building fromJson(const nlohmann::json& json);

private:
    std::string name_;          // Display name (e.g., "Stone Wall")
    BuildingType type_;         // Building type enum
    int x_, y_;                // Grid position (0-4, 0-4 for 5x5 grid)
    int durability_;           // Current durability (0 = destroyed)
    int maxDurability_;        // Maximum durability when fully repaired
    int defenseValue_;         // Defense points contributed to base
    
    void validateDurability();  // Ensures durability stays within bounds
};

// Helper functions for building type properties
namespace BuildingTypeHelper {
    std::string getTypeName(BuildingType type);
    int getDefaultDurability(BuildingType type);
    int getDefaultDefense(BuildingType type);
    std::string getRequiredCard(BuildingType type);  // e.g., "Wood" for WALL
    bool canPlaceMultiple(BuildingType type);        // Some buildings are unique
}
