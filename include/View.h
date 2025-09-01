#pragma once
#include "IGameView.h"
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
#include <unordered_map>

/**
 * Pure presentation layer View class
 * Implements IGameView interface
 * Follows Single Responsibility Principle - only handles rendering and UI state queries
 * No business logic or event handling
 */
class View : public IGameView {
public:
    View(SDLManager& sdl);
    ~View() = default;

    // IGameView interface implementation - pure presentation
    void render(const Inventory& inventory, 
               const Card* selectedCard, 
               int mouseX, 
               int mouseY, 
               bool showCraftingPanel, 
               const CraftingSystem& craftingSystem) override;
    
    const Card* getHoveredCard(const Inventory& inventory, int mouseX, int mouseY) const override;
    bool isPointInUIArea(int x, int y, const std::string& areaName) const override;
    int getClickedRecipeIndex(int mouseX, int mouseY) const override;
    bool isButtonHovered(const std::string& buttonName, int mouseX, int mouseY) const override;
    bool isCraftingPanelHovered(int mouseX, int mouseY) const override;

private:
    SDLManager& sdlManager_;
    
    // UI Components - pure presentation
    std::vector<std::unique_ptr<UIButton>> buttons_;
    std::vector<std::unique_ptr<UICard>> cards_;
    std::unique_ptr<UITooltip> tooltip_;
    std::unique_ptr<UICraftingPanel> craftingPanel_;
    
    // UI area definitions for hit testing
    std::unordered_map<std::string, SDL_Rect> uiAreas_;
    
    // Component management - pure rendering logic
    void createButtons();
    void updateCards(const Inventory& inventory);
    void renderBackground();
    void renderHints();
    void updateTooltip(const Inventory& inventory, 
                      const Card* selectedCard, 
                      bool showCraftingPanel, 
                      int mouseX, 
                      int mouseY);
    
    // UI state helpers - no business logic
    void initializeUIAreas();
    SDL_Rect getButtonRect(const std::string& buttonName) const;
    SDL_Rect getCraftingPanelRect() const;
};