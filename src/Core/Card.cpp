#include "Core/Card.h"
#include "utils.h"
#include "UIConstants.h"  // For tooltip text constants

TooltipData Card::getTooltipData() const {
    TooltipData data;
    
    // Set title and subtitle
    data.title = name;
    data.subtitle = getTypeString();
    
    // Add rarity information
    std::string rarityText = std::to_string(rarity);
    if (rarity == 1) {
        rarityText += " (" + UIConstants::TOOLTIP_COMMON + ")";
    } else if (rarity == 2) {
        rarityText += " (" + UIConstants::TOOLTIP_RARE + ")";  
    } else {
        rarityText += " (" + UIConstants::TOOLTIP_LEGENDARY + ")";
    }
    data.addAttribute(UIConstants::TOOLTIP_RARITY, rarityText);
    
    // Add quantity
    data.addAttribute(UIConstants::TOOLTIP_QUANTITY, std::to_string(quantity));
    
    // Add weight information
    if (hasAttribute(AttributeType::WEIGHT)) {
        data.addAttribute(UIConstants::TOOLTIP_WEIGHT, 
                         to_string_with_precision(getAttribute(AttributeType::WEIGHT), 2), "kg");
        if (quantity > 1) {
            data.addAttribute(UIConstants::TOOLTIP_TOTAL_WEIGHT,
                             to_string_with_precision(getTotalWeight(), 2), "kg");
        }
    }
    
    // Add other attributes
    if (hasAttribute(AttributeType::NUTRITION)) {
        data.addAttribute(UIConstants::TOOLTIP_NUTRITION, 
                         std::to_string(static_cast<int>(getAttribute(AttributeType::NUTRITION))));
    }
    
    if (hasAttribute(AttributeType::ATTACK)) {
        data.addAttribute(UIConstants::TOOLTIP_ATTACK,
                         std::to_string(static_cast<int>(getAttribute(AttributeType::ATTACK))));
    }
    
    if (hasAttribute(AttributeType::DEFENSE)) {
        data.addAttribute(UIConstants::TOOLTIP_DEFENSE,
                         std::to_string(static_cast<int>(getAttribute(AttributeType::DEFENSE))));
    }
    
    if (hasAttribute(AttributeType::HEALING)) {
        data.addAttribute(UIConstants::TOOLTIP_HEALING,
                         std::to_string(static_cast<int>(getAttribute(AttributeType::HEALING))));
    }
    
    if (hasAttribute(AttributeType::DURABILITY)) {
        data.addAttribute(UIConstants::TOOLTIP_DURABILITY,
                         std::to_string(static_cast<int>(getAttribute(AttributeType::DURABILITY))));
    }
    
    if (hasAttribute(AttributeType::BURN_VALUE)) {
        data.addAttribute(UIConstants::TOOLTIP_BURN_VALUE,
                         std::to_string(static_cast<int>(getAttribute(AttributeType::BURN_VALUE))));
    }
    
    if (hasAttribute(AttributeType::CRAFTING_VALUE)) {
        data.addAttribute(UIConstants::TOOLTIP_CRAFTING_VALUE,
                         std::to_string(static_cast<int>(getAttribute(AttributeType::CRAFTING_VALUE))));
    }
    
    if (hasAttribute(AttributeType::TRADE_VALUE)) {
        data.addAttribute(UIConstants::TOOLTIP_TRADE_VALUE,
                         std::to_string(static_cast<int>(getAttribute(AttributeType::TRADE_VALUE))));
    }
    
    // Add functional tags
    if (isEdible()) {
        data.addTag(UIConstants::TOOLTIP_EDIBLE);
    }
    
    if (isBurnable()) {
        data.addTag(UIConstants::TOOLTIP_BURNABLE);
    }
    
    return data;
}

CardDisplayData Card::getCardDisplayData() const {
    CardDisplayData data;
    
    data.name = name;
    data.type = getTypeString();
    data.quantity = quantity;
    data.rarity = rarity;
    
    // Use default rarity-based colors (don't set custom colors)
    data.clearCustomColors();
    
    return data;
}

bool Card::isEquivalentForDisplay(const ICardComparable& other) const {
    // Try to cast to Card for comparison
    if (const Card* otherCard = dynamic_cast<const Card*>(&other)) {
        return (name == otherCard->name &&
                rarity == otherCard->rarity &&
                quantity == otherCard->quantity &&
                type == otherCard->type);
    }
    return false;
}
