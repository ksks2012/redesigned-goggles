#include "Core/Building.h"
#include <algorithm>
#include <stdexcept>

Building::Building(const std::string& name, BuildingType type, int x, int y, 
                   int maxDurability, int defenseValue)
    : name_(name), type_(type), x_(x), y_(y), 
      durability_(maxDurability), maxDurability_(maxDurability), defenseValue_(defenseValue) {
    validateDurability();
}

void Building::takeDamage(int damage) {
    if (damage < 0) return;  // Ignore negative damage
    
    durability_ = std::max(0, durability_ - damage);
    validateDurability();
}

void Building::repair(int amount) {
    if (amount < 0) return;  // Ignore negative repair
    
    durability_ = std::min(maxDurability_, durability_ + amount);
    validateDurability();
}

float Building::getDurabilityPercentage() const {
    if (maxDurability_ == 0) return 0.0f;
    return static_cast<float>(durability_) / static_cast<float>(maxDurability_);
}

void Building::validateDurability() {
    durability_ = std::clamp(durability_, 0, maxDurability_);
}

nlohmann::json Building::toJson() const {
    return nlohmann::json{
        {"name", name_},
        {"type", static_cast<int>(type_)},
        {"x", x_},
        {"y", y_},
        {"durability", durability_},
        {"maxDurability", maxDurability_},
        {"defenseValue", defenseValue_}
    };
}

Building Building::fromJson(const nlohmann::json& json) {
    Building building(
        json["name"].get<std::string>(),
        static_cast<BuildingType>(json["type"].get<int>()),
        json["x"].get<int>(),
        json["y"].get<int>(),
        json["maxDurability"].get<int>(),
        json["defenseValue"].get<int>()
    );
    
    building.durability_ = json["durability"].get<int>();
    building.validateDurability();
    
    return building;
}

// BuildingTypeHelper implementation
// TODO: JSON or config-based definitions for extensibility
namespace BuildingTypeHelper {
    std::string getTypeName(BuildingType type) {
        switch (type) {
            case BuildingType::WALL: return "Wall";
            case BuildingType::FARM: return "Farm";
            case BuildingType::WORKSHOP: return "Workshop";
            case BuildingType::STORAGE: return "Storage";
            case BuildingType::WATCHTOWER: return "Watchtower";
            default: return "Unknown";
        }
    }

    int getDefaultDurability(BuildingType type) {
        switch (type) {
            case BuildingType::WALL: return 100;
            case BuildingType::FARM: return 80;
            case BuildingType::WORKSHOP: return 60;
            case BuildingType::STORAGE: return 40;
            case BuildingType::WATCHTOWER: return 120;
            default: return 50;
        }
    }

    int getDefaultDefense(BuildingType type) {
        switch (type) {
            case BuildingType::WALL: return 10;
            case BuildingType::FARM: return 0;
            case BuildingType::WORKSHOP: return 0;
            case BuildingType::STORAGE: return 0;
            case BuildingType::WATCHTOWER: return 5;
            default: return 0;
        }
    }

    std::string getRequiredCard(BuildingType type) {
        switch (type) {
            case BuildingType::WALL: return "Wood";
            case BuildingType::FARM: return "Seed";
            case BuildingType::WORKSHOP: return "Metal";
            case BuildingType::STORAGE: return "Leather";
            case BuildingType::WATCHTOWER: return "Stone";
            default: return "";
        }
    }

    bool canPlaceMultiple(BuildingType type) {
        switch (type) {
            case BuildingType::WALL: return true;
            case BuildingType::FARM: return true;
            case BuildingType::WORKSHOP: return false;  // Only one workshop allowed
            case BuildingType::STORAGE: return true;
            case BuildingType::WATCHTOWER: return false; // Only one watchtower allowed
            default: return true;
        }
    }
}
