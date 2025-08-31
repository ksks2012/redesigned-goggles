#pragma once
#include "SDLManager.h"
#include "Inventory.h"
#include "Card.h"
#include "CraftingSystem.h"
#include "ui/UIButton.h"
#include "ui/UICard.h"
#include "ui/UITooltip.h"
#include "ui/UICraftingPanel.h"
#include <vector>
#include <memory>
#include <functional>

/**
 * Refactored View class using the new UI component framework
 * Manages all UI components and their interactions
 */
class View {
public:
    View(SDLManager& sdl);
    ~View() = default;

    // Main rendering function
    void render(const Inventory& inventory, const Card* selectedCard, int mouseX, int mouseY, 
                bool showCraftingPanel, const CraftingSystem& craftingSystem);

    // TODO: Event handling
    void handleClick(int mouseX, int mouseY);
    
    // Card interaction
    const Card* getHoveredCard(const Inventory& inventory, int mouseX, int mouseY);
    
    // Button callbacks setup
    void setButtonCallbacks(
        std::function<void()> onAddCard,
        std::function<void()> onRemoveCard,
        std::function<void()> onExplore,
        std::function<void()> onCraft
    );
    
    // Crafting panel callbacks
    void setCraftingCallback(std::function<void(const Recipe&)> onRecipeCraft);

private:
    SDLManager& sdlManager_;
    
    // UI Components
    std::vector<std::unique_ptr<UIButton>> buttons_;
    std::vector<std::unique_ptr<UICard>> cards_;
    std::unique_ptr<UITooltip> tooltip_;
    std::unique_ptr<UICraftingPanel> craftingPanel_;
    
    // Component management
    void createButtons();
    void updateCards(const Inventory& inventory);
    void renderBackground();
    void renderHints();
    void handleTooltip(const Inventory& inventory, const Card* selectedCard, 
                      bool showCraftingPanel, int mouseX, int mouseY);
    
    // Button callbacks
    std::function<void()> onAddCard_;
    std::function<void()> onRemoveCard_;
    std::function<void()> onExplore_;
    std::function<void()> onCraft_;
};