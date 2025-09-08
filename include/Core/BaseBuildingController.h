#pragma once
#include "Core/BaseManager.h"
#include "Core/Inventory.h"
#include "Core/Card.h"
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

/**
 * Controller for base building operations
 * Handles drag-and-drop logic for cards to base area
 * Manages building placement validation and grid calculations
 * Implements background durability decay system with thread safety
 */
class BaseBuildingController {
public:
    BaseBuildingController(BaseManager& baseManager, Inventory& inventory);
    ~BaseBuildingController();
    
    // Core drag and drop functionality
    bool handleCardDrop(const Card* draggedCard, int mouseX, int mouseY);
    bool isInBaseArea(int mouseX, int mouseY) const;
    std::pair<int, int> calculateGridPosition(int mouseX, int mouseY) const;
    
    // Building placement validation
    bool canPlaceBuilding(const Card* card, int gridX, int gridY) const;
    bool validateBuildingRules(BuildingType buildingType, int gridX, int gridY) const;
    
    // Building placement execution
    bool placeBuildingFromCard(const Card* card, int gridX, int gridY);
    
    // Background durability management
    void startDurabilityDecay();
    void stopDurabilityDecay();
    
    // Building dependency rules (strategy depth)
    bool checkBuildingDependencies(BuildingType buildingType, int gridX, int gridY) const;
    bool hasAdjacentWaterSource(int gridX, int gridY) const;
    bool hasAdjacentBuilding(int gridX, int gridY, BuildingType requiredType) const;
    
    // Error reporting and feedback
    enum class PlacementError {
        NONE,
        INVALID_POSITION,
        GRID_OCCUPIED,
        INSUFFICIENT_MATERIALS,
        INVALID_CARD_TYPE,
        DEPENDENCY_NOT_MET,
        GRID_NOT_UNLOCKED
    };
    
    PlacementError getLastError() const { return lastError_; }
    std::string getErrorMessage(PlacementError error) const;
    
    // Callback for UI notifications (e.g., log messages or UI feedback)
    void setNotificationCallback(std::function<void(const std::string&)> callback);

private:
    BaseManager& baseManager_;
    Inventory& inventory_;
    
    // Thread safety for background operations
    std::mutex durabilityMutex_;
    std::atomic<bool> durabilityThreadActive_;
    std::thread durabilityThread_;
    
    // Error tracking
    mutable PlacementError lastError_;
    
    // Notification system
    std::function<void(const std::string&)> notificationCallback_;
    
    // Background durability decay worker
    void durabilityDecayWorker();
    
    // Helper methods
    bool consumeCardFromInventory(const Card* card);
    void notifyUser(const std::string& message);
    
    // Building rule validation helpers
    bool isValidGridPosition(int gridX, int gridY) const;
    bool isGridSlotAvailable(int gridX, int gridY) const;
    BuildingType getCardBuildingType(const Card* card) const;
};
