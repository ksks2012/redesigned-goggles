#include "Core/BaseBuildingController.h"
#include "Constants.h"
#include <iostream>
#include <algorithm>

BaseBuildingController::BaseBuildingController(BaseManager& baseManager, Inventory& inventory)
    : baseManager_(baseManager), inventory_(inventory), 
      durabilityThreadActive_(false), lastError_(PlacementError::NONE) {
}

BaseBuildingController::~BaseBuildingController() {
    stopDurabilityDecay();
}

bool BaseBuildingController::handleCardDrop(const Card* draggedCard, int mouseX, int mouseY) {
    if (!draggedCard) {
        lastError_ = PlacementError::INVALID_CARD_TYPE;
        return false;
    }
    
    // Check if drop position is in base area
    if (!isInBaseArea(mouseX, mouseY)) {
        lastError_ = PlacementError::INVALID_POSITION;
        notifyUser("Cannot place building outside base area");
        return false;
    }
    
    // Calculate grid position
    auto [gridX, gridY] = calculateGridPosition(mouseX, mouseY);
    
    // Validate building placement
    if (!canPlaceBuilding(draggedCard, gridX, gridY)) {
        notifyUser("Cannot place " + draggedCard->name + " at this location: " + 
                  getErrorMessage(lastError_));
        return false;
    }
    
    // Execute building placement
    bool success = placeBuildingFromCard(draggedCard, gridX, gridY);
    if (success) {
        notifyUser("Successfully placed " + draggedCard->name + " at grid (" + 
                  std::to_string(gridX) + ", " + std::to_string(gridY) + ")");
        lastError_ = PlacementError::NONE;
    } else {
        notifyUser("Failed to place building due to internal error");
    }
    
    return success;
}

bool BaseBuildingController::isInBaseArea(int mouseX, int mouseY) const {
    return mouseX >= Constants::BASE_AREA_START_X && mouseX <= Constants::BASE_AREA_END_X &&
           mouseY >= Constants::BASE_AREA_START_Y && mouseY <= Constants::BASE_AREA_END_Y;
}

std::pair<int, int> BaseBuildingController::calculateGridPosition(int mouseX, int mouseY) const {
    int gridX = (mouseX - Constants::BASE_AREA_START_X) / Constants::GRID_CELL_WIDTH;
    int gridY = mouseY / Constants::GRID_CELL_HEIGHT;
    
    // Clamp to valid grid range
    gridX = std::max(0, std::min(Constants::GRID_SIZE - 1, gridX));
    gridY = std::max(0, std::min(Constants::GRID_SIZE - 1, gridY));
    
    return {gridX, gridY};
}

bool BaseBuildingController::canPlaceBuilding(const Card* card, int gridX, int gridY) const {
    // Check if position is valid
    if (!isValidGridPosition(gridX, gridY)) {
        lastError_ = PlacementError::INVALID_POSITION;
        return false;
    }
    
    // Check if grid slot is unlocked
    if (!baseManager_.isSlotUnlocked(gridX, gridY)) {
        lastError_ = PlacementError::GRID_NOT_UNLOCKED;
        return false;
    }
    
    // Check if grid slot is available (not occupied)
    if (!isGridSlotAvailable(gridX, gridY)) {
        lastError_ = PlacementError::GRID_OCCUPIED;
        return false;
    }
    
    // Check if card can be used for building
    BuildingType buildingType = getCardBuildingType(card);
    if (buildingType == BuildingType::NONE) {
        lastError_ = PlacementError::INVALID_CARD_TYPE;
        return false;
    }
    
    // Check building-specific rules and dependencies
    if (!validateBuildingRules(buildingType, gridX, gridY)) {
        lastError_ = PlacementError::DEPENDENCY_NOT_MET;
        return false;
    }
    
    // Check if player has sufficient materials
    if (card->quantity <= 0) {
        lastError_ = PlacementError::INSUFFICIENT_MATERIALS;
        return false;
    }
    
    return true;
}

bool BaseBuildingController::validateBuildingRules(BuildingType buildingType, int gridX, int gridY) const {
    return checkBuildingDependencies(buildingType, gridX, gridY);
}

bool BaseBuildingController::placeBuildingFromCard(const Card* card, int gridX, int gridY) {
    // Final validation before placement
    if (!canPlaceBuilding(card, gridX, gridY)) {
        return false;
    }
    
    // Try to place building using BaseManager
    bool placementSuccess = baseManager_.placeBuilding(gridX, gridY, card->name, inventory_);
    
    if (placementSuccess) {
        notifyUser("Building placed successfully!");
        return true;
    } else {
        lastError_ = PlacementError::INSUFFICIENT_MATERIALS;
        return false;
    }
}

void BaseBuildingController::startDurabilityDecay() {
    if (durabilityThreadActive_.load()) {
        return; // Already running
    }
    
    durabilityThreadActive_.store(true);
    durabilityThread_ = std::thread(&BaseBuildingController::durabilityDecayWorker, this);
    notifyUser("Building durability decay system started");
}

void BaseBuildingController::stopDurabilityDecay() {
    if (durabilityThreadActive_.load()) {
        durabilityThreadActive_.store(false);
        if (durabilityThread_.joinable()) {
            durabilityThread_.join();
        }
        notifyUser("Building durability decay system stopped");
    }
}

bool BaseBuildingController::checkBuildingDependencies(BuildingType buildingType, int gridX, int gridY) const {
    switch (buildingType) {
        case BuildingType::FARM:
            // Farm requires adjacent water source or another farm
            return hasAdjacentWaterSource(gridX, gridY) || 
                   hasAdjacentBuilding(gridX, gridY, BuildingType::FARM);
        
        case BuildingType::WORKSHOP:
            // Workshop requires adjacent wall for protection
            return hasAdjacentBuilding(gridX, gridY, BuildingType::WALL);
        
        case BuildingType::STORAGE:
            // Storage can be placed freely but benefits from wall protection
            return true;
        
        case BuildingType::WALL:
            // Walls can be placed freely
            return true;
        
        case BuildingType::WATCHTOWER:
            // Watchtower requires at least one adjacent wall
            return hasAdjacentBuilding(gridX, gridY, BuildingType::WALL);
        
        default:
            return true;
    }
}

bool BaseBuildingController::hasAdjacentWaterSource(int gridX, int gridY) const {
    // For now, we'll simulate water sources at specific locations
    // In a full implementation, this could check for water tiles or buildings
    
    // Check adjacent positions for water or farm buildings
    const std::vector<std::pair<int, int>> adjacentPositions = {
        {gridX - 1, gridY}, {gridX + 1, gridY},
        {gridX, gridY - 1}, {gridX, gridY + 1}
    };
    
    for (const auto& [x, y] : adjacentPositions) {
        if (isValidGridPosition(x, y)) {
            Building* building = baseManager_.getBuildingAt(x, y);
            if (building && building->getType() == BuildingType::FARM) {
                return true; // Adjacent farm implies water access
            }
        }
    }
    
    // Simulate edge water sources (grid edges have water access)
    return gridX == 0 || gridY == 0 || 
           gridX == Constants::GRID_SIZE - 1 || 
           gridY == Constants::GRID_SIZE - 1;
}

bool BaseBuildingController::hasAdjacentBuilding(int gridX, int gridY, BuildingType requiredType) const {
    const std::vector<std::pair<int, int>> adjacentPositions = {
        {gridX - 1, gridY}, {gridX + 1, gridY},
        {gridX, gridY - 1}, {gridX, gridY + 1}
    };
    
    for (const auto& [x, y] : adjacentPositions) {
        if (isValidGridPosition(x, y)) {
            Building* building = baseManager_.getBuildingAt(x, y);
            if (building && building->getType() == requiredType && building->isOperational()) {
                return true;
            }
        }
    }
    
    return false;
}

std::string BaseBuildingController::getErrorMessage(PlacementError error) const {
    switch (error) {
        case PlacementError::NONE:
            return "No error";
        case PlacementError::INVALID_POSITION:
            return "Invalid position for building placement";
        case PlacementError::GRID_OCCUPIED:
            return "Grid position is already occupied";
        case PlacementError::INSUFFICIENT_MATERIALS:
            return "Not enough materials to build";
        case PlacementError::INVALID_CARD_TYPE:
            return "This card cannot be used for building";
        case PlacementError::DEPENDENCY_NOT_MET:
            return "Building dependency requirements not met";
        case PlacementError::GRID_NOT_UNLOCKED:
            return "Grid position is not unlocked";
        default:
            return "Unknown error";
    }
}

void BaseBuildingController::setNotificationCallback(std::function<void(const std::string&)> callback) {
    notificationCallback_ = callback;
}

void BaseBuildingController::durabilityDecayWorker() {
    while (durabilityThreadActive_.load()) {
        // Wait for decay interval
        std::this_thread::sleep_for(
            std::chrono::milliseconds(Constants::DURABILITY_DECAY_INTERVAL_MS));
        
        if (!durabilityThreadActive_.load()) break;
        
        // Apply durability decay to all buildings
        {
            std::lock_guard<std::mutex> lock(durabilityMutex_);
            
            int damagedBuildings = 0;
            
            // Iterate through all grid positions to find buildings
            for (int x = 0; x < Constants::GRID_SIZE; x++) {
                for (int y = 0; y < Constants::GRID_SIZE; y++) {
                    Building* building = baseManager_.getBuildingAt(x, y);
                    if (building && building->isOperational()) {
                        int currentDurability = building->getDurability();
                        int maxDurability = building->getMaxDurability();
                        int decayAmount = static_cast<int>(maxDurability * Constants::DURABILITY_DECAY_RATE);
                        decayAmount = std::max(1, decayAmount); // Minimum 1 point decay
                        
                        building->takeDamage(decayAmount);
                        damagedBuildings++;
                    }
                }
            }
            
            if (damagedBuildings > 0) {
                notifyUser("Building maintenance: " + std::to_string(damagedBuildings) + 
                          " buildings lost durability due to aging");
            }
        }
    }
}

bool BaseBuildingController::consumeCardFromInventory(const Card* card) {
    if (!card || card->quantity <= 0) {
        return false;
    }
    
    // This should be handled by BaseManager::placeBuilding, but we can add validation here
    auto cards = inventory_.getCards();
    for (auto& inventoryCard : cards) {
        if (inventoryCard.name == card->name && inventoryCard.rarity == card->rarity) {
            return inventoryCard.quantity > 0;
        }
    }
    
    return false;
}

void BaseBuildingController::notifyUser(const std::string& message) {
    if (notificationCallback_) {
        notificationCallback_(message);
    } else {
        // Fallback to console output
        std::cout << "[BaseBuildingController] " << message << std::endl;
    }
}

bool BaseBuildingController::isValidGridPosition(int gridX, int gridY) const {
    return baseManager_.isPositionValid(gridX, gridY);
}

bool BaseBuildingController::isGridSlotAvailable(int gridX, int gridY) const {
    return baseManager_.getBuildingAt(gridX, gridY) == nullptr;
}

BuildingType BaseBuildingController::getCardBuildingType(const Card* card) const {
    if (!card) return BuildingType::NONE;
    
    // Use existing conversion system from BaseManager
    return BuildingConversion::cardToBuildingType(card->name);
}
