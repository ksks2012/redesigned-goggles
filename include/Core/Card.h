#pragma once
#include <string>
#include <unordered_map>
#include "Interface/ui/TooltipData.h"
#include "Interface/ui/CardDisplayData.h"

// Card type enumeration
enum class CardType {
    METAL,      // Metal
    FOOD,       // Food
    HERB,       // Herb
    TOOL,       // Tool
    FUEL,       // Fuel
    BUILDING,   // Building material
    WEAPON,     // Weapon
    ARMOR,      // Armor
    MISC        // Miscellaneous
};

// Attribute type enumeration
enum class AttributeType {
    WEIGHT,
    DURABILITY,
    NUTRITION,
    BURN_VALUE,
    HEALING,
    ATTACK,
    DEFENSE,
    CRAFTING_VALUE,
    TRADE_VALUE
};

struct Card : public ITooltipProvider, public ICardDisplayProvider, public ICardComparable {
    std::string name;
    int rarity; // 1=Common, 2=Rare, 3=Legendary
    int quantity;
    CardType type;
    std::unordered_map<AttributeType, float> attributes;

    // Basic constructor
    Card(const std::string& n, int r, CardType t, int q = 1) 
        : name(n), rarity(r), quantity(q), type(t) {}
    
    // Backward compatible constructor (temporarily retained)
    Card(const std::string& n, int r, int q = 1) 
        : name(n), rarity(r), quantity(q), type(CardType::MISC) {}

    // Set attribute value
    void setAttribute(AttributeType attrType, float value) {
        attributes[attrType] = value;
    }

    // Get attribute value
    float getAttribute(AttributeType attrType) const {
        auto it = attributes.find(attrType);
        return (it != attributes.end()) ? it->second : 0.0f;
    }

    // Get attribute value with custom default
    float getAttribute(AttributeType attrType, float defaultValue) const {
        auto it = attributes.find(attrType);
        return (it != attributes.end()) ? it->second : defaultValue;
    }

    // Check if attribute exists
    bool hasAttribute(AttributeType attrType) const {
        return attributes.find(attrType) != attributes.end();
    }

    // Get type string
    std::string getTypeString() const {
        switch (type) {
            case CardType::METAL: return "Metal";
            case CardType::FOOD: return "Food";
            case CardType::HERB: return "Herb";
            case CardType::TOOL: return "Tool";
            case CardType::FUEL: return "Fuel";
            case CardType::BUILDING: return "Building Material";
            case CardType::WEAPON: return "Weapon";
            case CardType::ARMOR: return "Armor";
            case CardType::MISC: return "Miscellaneous";
            default: return "Unknown";
        }
    }

    // Get attribute string description
    std::string getAttributeString(AttributeType attrType) const {
        switch (attrType) {
            case AttributeType::WEIGHT: return "Weight";
            case AttributeType::DURABILITY: return "Durability";
            case AttributeType::NUTRITION: return "Nutrition";
            case AttributeType::BURN_VALUE: return "Burn Value";
            case AttributeType::HEALING: return "Healing";
            case AttributeType::ATTACK: return "Attack";
            case AttributeType::DEFENSE: return "Defense";
            case AttributeType::CRAFTING_VALUE: return "Crafting Value";
            case AttributeType::TRADE_VALUE: return "Trade Value";
            default: return "Unknown Attribute";
        }
    }

    // Calculate total weight (quantity * unit weight)
    float getTotalWeight() const {
        return getAttribute(AttributeType::WEIGHT) * quantity;
    }

    // Check if card is edible
    bool isEdible() const {
        return type == CardType::FOOD || type == CardType::HERB;
    }

    // Check if card is burnable
    bool isBurnable() const {
        return hasAttribute(AttributeType::BURN_VALUE) && getAttribute(AttributeType::BURN_VALUE) > 0;
    }
    
    // Implement ITooltipProvider interface
    TooltipData getTooltipData() const override;
    
    // Implement ICardDisplayProvider interface
    CardDisplayData getCardDisplayData() const override;
    
    // Implement ICardComparable interface
    bool isEquivalentForDisplay(const ICardComparable& other) const override;

    bool compare(const CardDisplayData& other) const {
        return (name == other.name &&
                rarity == other.rarity &&
                quantity == other.quantity);
    }

    bool compare(const Card& other) const {
        return (name == other.name &&
                rarity == other.rarity &&
                quantity == other.quantity);
    }
};