#pragma once
#include "Building.h"
#include "Constants.h"
#include "Inventory.h"
#include <vector>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>

/**
 * Base management system for grid-based building placement
 * Manages a 5x5 expandable grid where players can construct buildings
 * Integrates with inventory system for card-to-building conversion
 */
class BaseManager {
public:
    BaseManager();
    ~BaseManager() = default;

    // Building placement and removal
    bool canPlaceBuildingAt(int x, int y, BuildingType type) const;
    bool placeBuilding(int x, int y, const std::string& cardName, Inventory& inventory);
    bool removeBuildingAt(int x, int y);
    void repairBuilding(int x, int y, int repairAmount);

    // Grid and slot management
    bool isPositionValid(int x, int y) const;
    bool isSlotUnlocked(int x, int y) const;
    void unlockSlot(int x, int y);
    void unlockAdjacentSlots(int centerX, int centerY);
    int getUnlockedSlotCount() const;
    int getUsedSlotCount() const;

    // Building queries
    Building* getBuildingAt(int x, int y);
    const Building* getBuildingAt(int x, int y) const;
    std::vector<Building*> getAllBuildings();
    std::vector<const Building*> getAllBuildings() const;
    std::vector<Building*> getBuildingsByType(BuildingType type);

    // Base statistics
    int getTotalDefense() const;
    int getTotalDurability() const;
    int getMaxTotalDurability() const;
    float getBaseHealthPercentage() const;
    bool hasBuilding(BuildingType type) const;

    // Event handling (for future expansion)
    void takeDamageFromEvent(int totalDamage);  // Distribute damage across buildings
    void dailyMaintenance();                    // Daily durability loss/effects

    // Expansion system
    bool canExpandGrid() const;
    void expandGrid();  // Unlock more slots when conditions are met
    int getCurrentGridSize() const { return currentGridSize_; }

    // Serialization
    nlohmann::json toJson() const;
    void fromJson(const nlohmann::json& json);

    // Callbacks for UI updates
    void setOnBuildingPlaced(std::function<void(int, int, const Building&)> callback) {
        onBuildingPlaced_ = callback;
    }
    void setOnBuildingRemoved(std::function<void(int, int)> callback) {
        onBuildingRemoved_ = callback;
    }
    void setOnSlotUnlocked(std::function<void(int, int)> callback) {
        onSlotUnlocked_ = callback;
    }

private:
    // Grid state
    int currentGridSize_;
    std::vector<std::vector<bool>> unlockedSlots_;      // Which slots are available
    std::vector<std::vector<std::unique_ptr<Building>>> buildings_;  // Placed buildings

    // Event callbacks
    std::function<void(int, int, const Building&)> onBuildingPlaced_;
    std::function<void(int, int)> onBuildingRemoved_;
    std::function<void(int, int)> onSlotUnlocked_;

    // Helper methods
    void initializeGrid();
    void initializeDefaultUnlockedSlots();
    BuildingType getRequiredBuildingType(const std::string& cardName) const;
    bool hasRequiredResources(const std::string& cardName, const Inventory& inventory) const;
    void consumeResources(const std::string& cardName, Inventory& inventory);
    
    // Grid expansion logic
    bool meetsExpansionRequirements() const;
    void unlockExpansionSlots();
};

// Building conversion mappings
namespace BuildingConversion {
    // Map card names to building types
    BuildingType cardToBuildingType(const std::string& cardName);
    
    // Get required resources for building types
    std::vector<std::string> getRequiredCards(BuildingType type);
    
    // Check if card can be used for building
    bool isCardBuildable(const std::string& cardName);
}
