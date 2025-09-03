#include "../lib/catch2/catch.hpp"
#include "Core/Card.h"

TEST_CASE("Card creation and basic functionality", "[Card]") {
    SECTION("Basic card construction with type") {
        Card metalCard("Iron", 1, CardType::METAL, 5);
        
        REQUIRE(metalCard.name == "Iron");
        REQUIRE(metalCard.rarity == 1);
        REQUIRE(metalCard.quantity == 5);
        REQUIRE(metalCard.type == CardType::METAL);
    }
    
    SECTION("Backward compatible constructor") {
        Card legacyCard("Wood", 2, 3);
        
        REQUIRE(legacyCard.name == "Wood");
        REQUIRE(legacyCard.rarity == 2);
        REQUIRE(legacyCard.quantity == 3);
        REQUIRE(legacyCard.type == CardType::MISC);
    }
    
    SECTION("Card attribute management") {
        Card weapon("Sword", 2, CardType::WEAPON);
        
        // Set attributes
        weapon.setAttribute(AttributeType::ATTACK, 15.5f);
        weapon.setAttribute(AttributeType::DURABILITY, 100.0f);
        weapon.setAttribute(AttributeType::WEIGHT, 2.5f);
        
        // Verify attributes
        REQUIRE(weapon.getAttribute(AttributeType::ATTACK) == 15.5f);
        REQUIRE(weapon.getAttribute(AttributeType::DURABILITY) == 100.0f);
        REQUIRE(weapon.getAttribute(AttributeType::WEIGHT) == 2.5f);
    }
    
    SECTION("Card attribute with default value") {
        Card food("Apple", 1, CardType::FOOD);
        
        // Test getAttribute with default value
        REQUIRE(food.getAttribute(AttributeType::NUTRITION, 0.0f) == 0.0f);
        
        food.setAttribute(AttributeType::NUTRITION, 25.0f);
        REQUIRE(food.getAttribute(AttributeType::NUTRITION, 0.0f) == 25.0f);
    }
    
    SECTION("Card type verification") {
        Card tool("Hammer", 1, CardType::TOOL);
        Card herb("Healing Herb", 2, CardType::HERB);
        Card building("Wood Plank", 1, CardType::BUILDING);
        
        REQUIRE(tool.type == CardType::TOOL);
        REQUIRE(herb.type == CardType::HERB);
        REQUIRE(building.type == CardType::BUILDING);
    }
}

TEST_CASE("Card edge cases and validation", "[Card]") {
    SECTION("Zero and negative quantities") {
        Card zeroCard("Empty", 1, CardType::MISC, 0);
        REQUIRE(zeroCard.quantity == 0);
        
        Card negativeCard("Negative", 1, CardType::MISC, -5);
        REQUIRE(negativeCard.quantity == -5);
    }
    
    SECTION("High rarity values") {
        Card legendaryCard("Excalibur", 3, CardType::WEAPON);
        REQUIRE(legendaryCard.rarity == 3);
        
        Card ultraRareCard("Ancient Artifact", 10, CardType::MISC);
        REQUIRE(ultraRareCard.rarity == 10);
    }
    
    SECTION("Empty name handling") {
        Card emptyName("", 1, CardType::MISC);
        REQUIRE(emptyName.name == "");
    }
    
    SECTION("Multiple attribute modifications") {
        Card complexItem("Magic Staff", 3, CardType::WEAPON);
        
        // Set initial attributes
        complexItem.setAttribute(AttributeType::ATTACK, 20.0f);
        complexItem.setAttribute(AttributeType::DURABILITY, 80.0f);
        
        // Modify existing attributes
        complexItem.setAttribute(AttributeType::ATTACK, 25.0f);
        complexItem.setAttribute(AttributeType::DURABILITY, 75.0f);
        
        // Add new attribute
        complexItem.setAttribute(AttributeType::WEIGHT, 1.5f);
        
        REQUIRE(complexItem.getAttribute(AttributeType::ATTACK) == 25.0f);
        REQUIRE(complexItem.getAttribute(AttributeType::DURABILITY) == 75.0f);
        REQUIRE(complexItem.getAttribute(AttributeType::WEIGHT) == 1.5f);
    }
}
