#include "Core/BaseManager.h"
#include "Core/Building.h"
#include <algorithm>
#include <numeric>

BaseManager::BaseManager() : currentGridSize_(Constants::GRID_SIZE) {
    initializeGrid();
    initializeDefaultUnlockedSlots();
}

void BaseManager::initializeGrid() {
    unlockedSlots_.resize(Constants::MAX_GRID_SIZE);
    buildings_.resize(Constants::MAX_GRID_SIZE);
    
    for (int i = 0; i < Constants::MAX_GRID_SIZE; ++i) {
        unlockedSlots_[i].resize(Constants::MAX_GRID_SIZE, false);
        buildings_[i].resize(Constants::MAX_GRID_SIZE);  // Default initialized to nullptr
    }
}

void BaseManager::initializeDefaultUnlockedSlots() {
    // Unlock center cross pattern initially (5 slots)
    int center = Constants::GRID_SIZE / 2;  // Center of 5x5 grid (index 2)
    
    unlockSlot(center, center);         // Center
    unlockSlot(center - 1, center);     // Left
    unlockSlot(center + 1, center);     // Right
    unlockSlot(center, center - 1);     // Up
    unlockSlot(center, center + 1);     // Down
}

bool BaseManager::isPositionValid(int x, int y) const {
    return x >= 0 && x < currentGridSize_ && y >= 0 && y < currentGridSize_;
}

bool BaseManager::isSlotUnlocked(int x, int y) const {
    if (!isPositionValid(x, y)) return false;
    return unlockedSlots_[x][y];
}

void BaseManager::unlockSlot(int x, int y) {
    if (isPositionValid(x, y) && !unlockedSlots_[x][y]) {
        unlockedSlots_[x][y] = true;
        if (onSlotUnlocked_) {
            onSlotUnlocked_(x, y);
        }
    }
}

bool BaseManager::canPlaceBuildingAt(int x, int y, BuildingType type) const {
    // Check if position is valid and unlocked
    if (!isPositionValid(x, y) || !isSlotUnlocked(x, y)) {
        return false;
    }
    
    // Check if slot is empty
    if (buildings_[x][y] != nullptr) {
        return false;
    }
    
    // Check if building type allows multiple instances
    if (!BuildingTypeHelper::canPlaceMultiple(type)) {
        if (hasBuilding(type)) {
            return false;  // Building type already exists and doesn't allow multiples
        }
    }
    
    return true;
}

bool BaseManager::placeBuilding(int x, int y, const std::string& cardName, Inventory& inventory) {
    // Get required building type from card
    BuildingType buildingType = getRequiredBuildingType(cardName);
    if (buildingType == BuildingType::NONE) {
        return false;  // Card is not buildable
    }
    
    // Check if we can place building at this position
    if (!canPlaceBuildingAt(x, y, buildingType)) {
        return false;
    }
    
    // Check if player has required resources
    if (!hasRequiredResources(cardName, inventory)) {
        return false;
    }
    
    // Consume resources from inventory
    consumeResources(cardName, inventory);
    
    // Create and place building
    std::string buildingName = BuildingTypeHelper::getTypeName(buildingType);
    int durability = BuildingTypeHelper::getDefaultDurability(buildingType);
    int defense = BuildingTypeHelper::getDefaultDefense(buildingType);
    
    buildings_[x][y] = std::make_unique<Building>(
        buildingName, buildingType, x, y, durability, defense
    );
    
    // Unlock adjacent slots if this is the first building in that area
    unlockAdjacentSlots(x, y);
    
    // Notify observers
    if (onBuildingPlaced_) {
        onBuildingPlaced_(x, y, *buildings_[x][y]);
    }
    
    return true;
}

bool BaseManager::removeBuildingAt(int x, int y) {
    if (!isPositionValid(x, y) || buildings_[x][y] == nullptr) {
        return false;
    }
    
    buildings_[x][y].reset();
    
    if (onBuildingRemoved_) {
        onBuildingRemoved_(x, y);
    }
    
    return true;
}

void BaseManager::unlockAdjacentSlots(int centerX, int centerY) {
    const int directions[][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};  // Left, Right, Up, Down
    
    for (const auto& dir : directions) {
        int newX = centerX + dir[0];
        int newY = centerY + dir[1];
        
        if (isPositionValid(newX, newY)) {
            unlockSlot(newX, newY);
        }
    }
}

Building* BaseManager::getBuildingAt(int x, int y) {
    if (!isPositionValid(x, y)) return nullptr;
    return buildings_[x][y].get();
}

const Building* BaseManager::getBuildingAt(int x, int y) const {
    if (!isPositionValid(x, y)) return nullptr;
    return buildings_[x][y].get();
}

std::vector<Building*> BaseManager::getAllBuildings() {
    std::vector<Building*> result;
    for (int x = 0; x < currentGridSize_; ++x) {
        for (int y = 0; y < currentGridSize_; ++y) {
            if (buildings_[x][y] != nullptr) {
                result.push_back(buildings_[x][y].get());
            }
        }
    }
    return result;
}

std::vector<const Building*> BaseManager::getAllBuildings() const {
    std::vector<const Building*> result;
    for (int x = 0; x < currentGridSize_; ++x) {
        for (int y = 0; y < currentGridSize_; ++y) {
            if (buildings_[x][y] != nullptr) {
                result.push_back(buildings_[x][y].get());
            }
        }
    }
    return result;
}

int BaseManager::getTotalDefense() const {
    int totalDefense = 0;
    for (const auto* building : getAllBuildings()) {
        totalDefense += building->getDefenseValue();
    }
    return totalDefense;
}

int BaseManager::getTotalDurability() const {
    int totalDurability = 0;
    for (const auto* building : getAllBuildings()) {
        totalDurability += building->getDurability();
    }
    return totalDurability;
}

bool BaseManager::hasBuilding(BuildingType type) const {
    for (const auto* building : getAllBuildings()) {
        if (building->getType() == type) {
            return true;
        }
    }
    return false;
}

int BaseManager::getUnlockedSlotCount() const {
    int count = 0;
    for (int x = 0; x < currentGridSize_; ++x) {
        for (int y = 0; y < currentGridSize_; ++y) {
            if (unlockedSlots_[x][y]) {
                count++;
            }
        }
    }
    return count;
}

int BaseManager::getUsedSlotCount() const {
    return static_cast<int>(getAllBuildings().size());
}

BuildingType BaseManager::getRequiredBuildingType(const std::string& cardName) const {
    return BuildingConversion::cardToBuildingType(cardName);
}

bool BaseManager::hasRequiredResources(const std::string& cardName, const Inventory& inventory) const {
    // For now, just check if the card exists in inventory
    // This can be expanded for multi-card requirements
    auto cards = inventory.getCards();
    return std::any_of(cards.begin(), cards.end(), 
                      [&cardName](const Card& card) { return card.name == cardName; });
}

void BaseManager::consumeResources(const std::string& cardName, Inventory& inventory) {
    // Remove one instance of the required card
    // For now, assume rarity 1 - this can be enhanced later
    inventory.removeCard(cardName, 1);
}

nlohmann::json BaseManager::toJson() const {
    nlohmann::json json;
    json["currentGridSize"] = currentGridSize_;
    
    // Save unlocked slots
    json["unlockedSlots"] = nlohmann::json::array();
    for (int x = 0; x < currentGridSize_; ++x) {
        for (int y = 0; y < currentGridSize_; ++y) {
            if (unlockedSlots_[x][y]) {
                json["unlockedSlots"].push_back({{"x", x}, {"y", y}});
            }
        }
    }
    
    // Save buildings
    json["buildings"] = nlohmann::json::array();
    for (const auto* building : getAllBuildings()) {
        json["buildings"].push_back(building->toJson());
    }
    
    return json;
}

void BaseManager::fromJson(const nlohmann::json& json) {
    currentGridSize_ = json.value("currentGridSize", Constants::GRID_SIZE);
    
    // Clear and reinitialize grid
    initializeGrid();
    
    // Restore unlocked slots
    if (json.contains("unlockedSlots")) {
        for (const auto& slot : json["unlockedSlots"]) {
            int x = slot["x"];
            int y = slot["y"];
            unlockSlot(x, y);
        }
    } else {
        initializeDefaultUnlockedSlots();
    }
    
    // Restore buildings
    if (json.contains("buildings")) {
        for (const auto& buildingJson : json["buildings"]) {
            Building building = Building::fromJson(buildingJson);
            int x = building.getX();
            int y = building.getY();
            
            if (isPositionValid(x, y)) {
                buildings_[x][y] = std::make_unique<Building>(std::move(building));
            }
        }
    }
}

// BuildingConversion namespace implementation
namespace BuildingConversion {
    BuildingType cardToBuildingType(const std::string& cardName) {
        if (cardName == "Wood") return BuildingType::WALL;
        if (cardName == "Seed") return BuildingType::FARM;
        if (cardName == "Metal") return BuildingType::WORKSHOP;
        if (cardName == "Leather") return BuildingType::STORAGE;
        if (cardName == "Stone") return BuildingType::WATCHTOWER;
        return BuildingType::NONE;
    }
    
    std::vector<std::string> getRequiredCards(BuildingType type) {
        std::string requiredCard = BuildingTypeHelper::getRequiredCard(type);
        if (requiredCard.empty()) {
            return {};
        }
        return {requiredCard};  // For now, each building requires only one card type
    }
    
    bool isCardBuildable(const std::string& cardName) {
        return cardToBuildingType(cardName) != BuildingType::NONE;
    }
}
