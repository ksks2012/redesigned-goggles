#pragma once
#include <string>
#include <SDL2/SDL.h>

/**
 * Generic card display data structure that can represent any item's card information
 * without depending on specific game objects like Card
 */
struct CardDisplayData {
    std::string name;           // Item name
    std::string type;           // Item type description
    int quantity;              // Item quantity
    int rarity;                // Rarity level (1=Common, 2=Rare, 3=Legendary)
    
    // Display properties
    std::string displayText;    // Custom display text (optional)
    SDL_Color backgroundColor;  // Custom background color (optional)
    SDL_Color textColor;       // Custom text color (optional)
    bool useCustomColors;      // Whether to use custom colors
    
    CardDisplayData() 
        : name(""), type(""), quantity(1), rarity(1),
          displayText(""), backgroundColor{100, 100, 100, 255}, 
          textColor{255, 255, 255, 255}, useCustomColors(false) {}
    
    CardDisplayData(const std::string& name, const std::string& type, 
                   int quantity = 1, int rarity = 1)
        : name(name), type(type), quantity(quantity), rarity(rarity),
          displayText(""), backgroundColor{100, 100, 100, 255},
          textColor{255, 255, 255, 255}, useCustomColors(false) {}
    
    // Get formatted display text
    std::string getFormattedDisplayText() const {
        if (!displayText.empty()) {
            return displayText;
        }
        return name + " x" + std::to_string(quantity) + " (" + type + ")";
    }
    
    // Set custom colors
    void setCustomColors(SDL_Color bg, SDL_Color text) {
        backgroundColor = bg;
        textColor = text;
        useCustomColors = true;
    }
    
    // Clear custom colors (use default rarity-based colors)
    void clearCustomColors() {
        useCustomColors = false;
    }
    
    // Equality comparison for display data
    bool operator==(const CardDisplayData& other) const {
        return name == other.name && 
               type == other.type &&
               quantity == other.quantity &&
               rarity == other.rarity;
    }
    
    // Inequality comparison for display data
    bool operator!=(const CardDisplayData& other) const {
        return !(*this == other);
    }
};

/**
 * Interface for objects that can provide card display data
 */
class ICardDisplayProvider {
public:
    virtual ~ICardDisplayProvider() = default;
    virtual CardDisplayData getCardDisplayData() const = 0;
};

/**
 * Interface for objects that can be compared for card display purposes
 */
class ICardComparable {
public:
    virtual ~ICardComparable() = default;
    virtual bool isEquivalentForDisplay(const ICardComparable& other) const = 0;
};
