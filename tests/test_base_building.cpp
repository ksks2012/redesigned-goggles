#include "../lib/catch2/catch.hpp"
#include "../include/Core/BaseManager.h"
#include "../include/Core/Building.h"
#include "../include/Core/Inventory.h"

TEST_CASE("Building Creation and Properties", "[building]") {
    SECTION("Building constructor sets properties correctly") {
        Building wall("Stone Wall", BuildingType::WALL, 2, 3, 100, 10);
        
        REQUIRE(wall.getName() == "Stone Wall");
        REQUIRE(wall.getType() == BuildingType::WALL);
        REQUIRE(wall.getX() == 2);
        REQUIRE(wall.getY() == 3);
        REQUIRE(wall.getDurability() == 100);
        REQUIRE(wall.getMaxDurability() == 100);
        REQUIRE(wall.getDefenseValue() == 10);
        REQUIRE(wall.isOperational());
        REQUIRE_FALSE(wall.isDestroyed());
    }
    
    SECTION("Building takes damage correctly") {
        Building wall("Wall", BuildingType::WALL, 0, 0, 100, 10);
        
        wall.takeDamage(30);
        REQUIRE(wall.getDurability() == 70);
        REQUIRE(wall.getDurabilityPercentage() == Approx(0.7));
        REQUIRE(wall.getDefenseValue() == 10);  // Still operational
        
        wall.takeDamage(80);  // More than remaining durability
        REQUIRE(wall.getDurability() == 0);
        REQUIRE(wall.isDestroyed());
        REQUIRE(wall.getDefenseValue() == 0);  // No defense when destroyed
    }
    
    SECTION("Building repair works correctly") {
        Building wall("Wall", BuildingType::WALL, 0, 0, 100, 10);
        
        wall.takeDamage(50);
        REQUIRE(wall.getDurability() == 50);
        
        wall.repair(30);
        REQUIRE(wall.getDurability() == 80);
        
        wall.repair(50);  // Over max durability
        REQUIRE(wall.getDurability() == 100);  // Clamped to max
    }
}

TEST_CASE("Building Type Helper Functions", "[building]") {
    SECTION("Building type properties are correct") {
        REQUIRE(BuildingTypeHelper::getTypeName(BuildingType::WALL) == "Wall");
        REQUIRE(BuildingTypeHelper::getTypeName(BuildingType::FARM) == "Farm");
        
        REQUIRE(BuildingTypeHelper::getDefaultDurability(BuildingType::WALL) == 100);
        REQUIRE(BuildingTypeHelper::getDefaultDurability(BuildingType::FARM) == 80);
        
        REQUIRE(BuildingTypeHelper::getDefaultDefense(BuildingType::WALL) == 10);
        REQUIRE(BuildingTypeHelper::getDefaultDefense(BuildingType::FARM) == 0);
        
        REQUIRE(BuildingTypeHelper::getRequiredCard(BuildingType::WALL) == "Wood");
        REQUIRE(BuildingTypeHelper::getRequiredCard(BuildingType::FARM) == "Seed");
        
        REQUIRE(BuildingTypeHelper::canPlaceMultiple(BuildingType::WALL) == true);
        REQUIRE(BuildingTypeHelper::canPlaceMultiple(BuildingType::WORKSHOP) == false);
    }
}

TEST_CASE("BaseManager Grid System", "[base_manager]") {
    BaseManager baseManager;
    
    SECTION("Initial grid setup is correct") {
        REQUIRE(baseManager.getCurrentGridSize() == 5);
        REQUIRE(baseManager.getUnlockedSlotCount() == 5);  // Initial cross pattern
        REQUIRE(baseManager.getUsedSlotCount() == 0);
        
        // Center and adjacent slots should be unlocked
        REQUIRE(baseManager.isSlotUnlocked(2, 2));  // Center
        REQUIRE(baseManager.isSlotUnlocked(1, 2));  // Left
        REQUIRE(baseManager.isSlotUnlocked(3, 2));  // Right
        REQUIRE(baseManager.isSlotUnlocked(2, 1));  // Up
        REQUIRE(baseManager.isSlotUnlocked(2, 3));  // Down
        
        // Corners should be locked initially
        REQUIRE_FALSE(baseManager.isSlotUnlocked(0, 0));
        REQUIRE_FALSE(baseManager.isSlotUnlocked(4, 4));
    }
    
    SECTION("Position validation works") {
        REQUIRE(baseManager.isPositionValid(0, 0));
        REQUIRE(baseManager.isPositionValid(4, 4));
        REQUIRE_FALSE(baseManager.isPositionValid(-1, 0));
        REQUIRE_FALSE(baseManager.isPositionValid(0, -1));
        REQUIRE_FALSE(baseManager.isPositionValid(5, 0));
        REQUIRE_FALSE(baseManager.isPositionValid(0, 5));
    }
}

TEST_CASE("BaseManager Building Placement", "[base_manager]") {
    BaseManager baseManager;
    Inventory inventory;
    
    // Add required cards to inventory
    inventory.addCard(Card("Wood", 5, CardType::BUILDING, 1));
    inventory.addCard(Card("Seed", 3, CardType::BUILDING, 1));
    inventory.addCard(Card("Metal", 2, CardType::METAL, 1));
    
    SECTION("Can place building with valid conditions") {
        REQUIRE(baseManager.canPlaceBuildingAt(2, 2, BuildingType::WALL));
        REQUIRE(baseManager.placeBuilding(2, 2, "Wood", inventory));
        
        Building* building = baseManager.getBuildingAt(2, 2);
        REQUIRE(building != nullptr);
        REQUIRE(building->getType() == BuildingType::WALL);
        REQUIRE(building->getName() == "Wall");
        REQUIRE(baseManager.getUsedSlotCount() == 1);
    }
    
    SECTION("Cannot place building on occupied slot") {
        baseManager.placeBuilding(2, 2, "Wood", inventory);
        REQUIRE_FALSE(baseManager.canPlaceBuildingAt(2, 2, BuildingType::FARM));
        REQUIRE_FALSE(baseManager.placeBuilding(2, 2, "Seed", inventory));
    }
    
    SECTION("Cannot place building on locked slot") {
        REQUIRE_FALSE(baseManager.isSlotUnlocked(0, 0));
        REQUIRE_FALSE(baseManager.canPlaceBuildingAt(0, 0, BuildingType::WALL));
        REQUIRE_FALSE(baseManager.placeBuilding(0, 0, "Wood", inventory));
    }
    
    SECTION("Cannot place unique building twice") {
        REQUIRE(baseManager.placeBuilding(2, 2, "Metal", inventory));  // Workshop
        REQUIRE_FALSE(baseManager.canPlaceBuildingAt(1, 2, BuildingType::WORKSHOP));
        REQUIRE_FALSE(baseManager.placeBuilding(1, 2, "Metal", inventory));
    }
    
    SECTION("Building placement unlocks adjacent slots") {
        int initialUnlocked = baseManager.getUnlockedSlotCount();
        baseManager.placeBuilding(2, 2, "Wood", inventory);
        
        // Adjacent slots should be unlocked (if they weren't already)
        REQUIRE(baseManager.isSlotUnlocked(1, 2));
        REQUIRE(baseManager.isSlotUnlocked(3, 2));
        REQUIRE(baseManager.isSlotUnlocked(2, 1));
        REQUIRE(baseManager.isSlotUnlocked(2, 3));
    }
    
    SECTION("Building removal works") {
        baseManager.placeBuilding(2, 2, "Wood", inventory);
        REQUIRE(baseManager.getBuildingAt(2, 2) != nullptr);
        
        REQUIRE(baseManager.removeBuildingAt(2, 2));
        REQUIRE(baseManager.getBuildingAt(2, 2) == nullptr);
        REQUIRE(baseManager.getUsedSlotCount() == 0);
    }
}

TEST_CASE("BaseManager Statistics", "[base_manager]") {
    BaseManager baseManager;
    Inventory inventory;
    
    // Add required cards
    inventory.addCard(Card("Wood", 5, CardType::BUILDING, 1));
    inventory.addCard(Card("Seed", 3, CardType::BUILDING, 1));
    
    SECTION("Base statistics calculate correctly") {
        REQUIRE(baseManager.getTotalDefense() == 0);
        REQUIRE(baseManager.getTotalDurability() == 0);
        
        // Place a wall (defense: 10, durability: 100)
        baseManager.placeBuilding(2, 2, "Wood", inventory);
        REQUIRE(baseManager.getTotalDefense() == 10);
        REQUIRE(baseManager.getTotalDurability() == 100);
        
        // Place a farm (defense: 0, durability: 80)
        baseManager.placeBuilding(1, 2, "Seed", inventory);
        REQUIRE(baseManager.getTotalDefense() == 10);  // Still 10 (wall only)
        REQUIRE(baseManager.getTotalDurability() == 180);  // 100 + 80
        
        // Damage a building
        Building* wall = baseManager.getBuildingAt(2, 2);
        wall->takeDamage(50);
        REQUIRE(baseManager.getTotalDurability() == 130);  // 50 + 80
    }
    
    SECTION("Building type queries work") {
        REQUIRE_FALSE(baseManager.hasBuilding(BuildingType::WALL));
        
        baseManager.placeBuilding(2, 2, "Wood", inventory);
        REQUIRE(baseManager.hasBuilding(BuildingType::WALL));
        REQUIRE_FALSE(baseManager.hasBuilding(BuildingType::FARM));
    }
}

TEST_CASE("Building Serialization", "[building][serialization]") {
    SECTION("Building JSON serialization works") {
        Building original("Test Wall", BuildingType::WALL, 2, 3, 100, 15);
        original.takeDamage(25);  // 75 durability remaining
        
        nlohmann::json json = original.toJson();
        Building restored = Building::fromJson(json);
        
        REQUIRE(restored.getName() == "Test Wall");
        REQUIRE(restored.getType() == BuildingType::WALL);
        REQUIRE(restored.getX() == 2);
        REQUIRE(restored.getY() == 3);
        REQUIRE(restored.getDurability() == 75);
        REQUIRE(restored.getMaxDurability() == 100);
        REQUIRE(restored.getDefenseValue() == 15);
    }
}

TEST_CASE("BaseManager Serialization", "[base_manager][serialization]") {
    BaseManager original;
    Inventory inventory;
    inventory.addCard(Card("Wood", 5, CardType::BUILDING, 1));
    inventory.addCard(Card("Seed", 3, CardType::BUILDING, 1));
    
    SECTION("BaseManager JSON serialization works") {
        // Place some buildings
        original.placeBuilding(2, 2, "Wood", inventory);
        original.placeBuilding(1, 2, "Seed", inventory);
        
        // Unlock additional slot
        original.unlockSlot(0, 2);
        
        nlohmann::json json = original.toJson();
        
        BaseManager restored;
        restored.fromJson(json);
        
        REQUIRE(restored.getCurrentGridSize() == original.getCurrentGridSize());
        REQUIRE(restored.getUnlockedSlotCount() == original.getUnlockedSlotCount());
        REQUIRE(restored.getUsedSlotCount() == original.getUsedSlotCount());
        REQUIRE(restored.getTotalDefense() == original.getTotalDefense());
        REQUIRE(restored.getTotalDurability() == original.getTotalDurability());
        
        // Check specific buildings
        Building* originalWall = original.getBuildingAt(2, 2);
        Building* restoredWall = restored.getBuildingAt(2, 2);
        REQUIRE(restoredWall != nullptr);
        REQUIRE(restoredWall->getType() == originalWall->getType());
        REQUIRE(restoredWall->getName() == originalWall->getName());
    }
}

TEST_CASE("Building Conversion System", "[building_conversion]") {
    SECTION("Card to building type conversion works") {
        REQUIRE(BuildingConversion::cardToBuildingType("Wood") == BuildingType::WALL);
        REQUIRE(BuildingConversion::cardToBuildingType("Seed") == BuildingType::FARM);
        REQUIRE(BuildingConversion::cardToBuildingType("Metal") == BuildingType::WORKSHOP);
        REQUIRE(BuildingConversion::cardToBuildingType("Leather") == BuildingType::STORAGE);
        REQUIRE(BuildingConversion::cardToBuildingType("Stone") == BuildingType::WATCHTOWER);
        REQUIRE(BuildingConversion::cardToBuildingType("UnknownCard") == BuildingType::NONE);
    }
    
    SECTION("Buildable card detection works") {
        REQUIRE(BuildingConversion::isCardBuildable("Wood"));
        REQUIRE(BuildingConversion::isCardBuildable("Seed"));
        REQUIRE(BuildingConversion::isCardBuildable("Metal"));
        REQUIRE_FALSE(BuildingConversion::isCardBuildable("Food"));
        REQUIRE_FALSE(BuildingConversion::isCardBuildable("UnknownCard"));
    }
}
