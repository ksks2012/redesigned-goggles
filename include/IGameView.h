#pragma once
#include "Inventory.h"
#include "Card.h"
#include "CraftingSystem.h"
#include <functional>

/**
 * Interface for game view operations
 * Follows Interface Segregation Principle (ISP)
 * Pure presentation layer - no business logic
 */
class IGameView {
public:
    virtual ~IGameView() = default;
    
    // Pure rendering operations
    virtual void render(const Inventory& inventory, 
                       const Card* selectedCard, 
                       int mouseX, 
                       int mouseY, 
                       bool showCraftingPanel, 
                       const CraftingSystem& craftingSystem,
                       int inventoryScrollOffset = 0,
                       int craftingScrollOffset = 0) = 0;
    
    // UI state queries - no business logic
    virtual const Card* getHoveredCard(const Inventory& inventory, int mouseX, int mouseY, int scrollOffset = 0) const = 0;
    virtual bool isPointInUIArea(int x, int y, const std::string& areaName) const = 0;
    virtual int getClickedRecipeIndex(int mouseX, int mouseY, int scrollOffset = 0) const = 0;
    
    // UI component state
    virtual bool isButtonHovered(const std::string& buttonName, int mouseX, int mouseY) const = 0;
    virtual bool isCraftingPanelHovered(int mouseX, int mouseY) const = 0;
};

/**
 * View event data structures
 * Pure data - no behavior
 */
namespace ViewEvents {
    enum class ClickType {
        ButtonAdd,
        ButtonRemove, 
        ButtonExplore,
        ButtonCraft,
        Card,
        Recipe,
        CraftingPanelClose,
        Background
    };
    
    struct ClickEvent {
        ClickType type;
        int x, y;
        int cardIndex = -1;
        int recipeIndex = -1;
        const Card* card = nullptr;
    };
    
    struct HoverEvent {
        int x, y;
        const Card* hoveredCard = nullptr;
        bool isOverButton = false;
        std::string buttonName;
    };
}
