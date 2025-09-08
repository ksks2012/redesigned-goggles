#pragma once
#include <string>
#include <vector>
#include <unordered_map>

/**
 * Generic tooltip data structure that can represent any item's tooltip information
 * without depending on specific game objects like Card
 */
struct TooltipData {
    struct AttributeInfo {
        std::string name;
        std::string value;
        std::string unit;  // Optional unit like "kg", "points", etc.
        
        AttributeInfo(const std::string& name, const std::string& value, const std::string& unit = "")
            : name(name), value(value), unit(unit) {}
    };
    
    struct TagInfo {
        std::string tag;
        
        TagInfo(const std::string& tag) : tag(tag) {}
    };
    
    std::string title;                              // Main title
    std::string subtitle;                          // Secondary title/type info
    std::vector<AttributeInfo> attributes;         // Key-value attribute pairs
    std::vector<TagInfo> tags;                     // Special tags/flags
    
    TooltipData() = default;
    
    TooltipData(const std::string& title, const std::string& subtitle = "")
        : title(title), subtitle(subtitle) {}
    
    // Convenience methods for adding data
    void addAttribute(const std::string& name, const std::string& value, const std::string& unit = "") {
        attributes.emplace_back(name, value, unit);
    }
    
    void addTag(const std::string& tag) {
        tags.emplace_back(tag);
    }
    
    // Clear all data
    void clear() {
        title.clear();
        subtitle.clear();
        attributes.clear();
        tags.clear();
    }
    
    // Check if tooltip has any content
    bool isEmpty() const {
        return title.empty() && subtitle.empty() && attributes.empty() && tags.empty();
    }
};

/**
 * Interface for objects that can provide tooltip data
 */
class ITooltipProvider {
public:
    virtual ~ITooltipProvider() = default;
    virtual TooltipData getTooltipData() const = 0;
};
