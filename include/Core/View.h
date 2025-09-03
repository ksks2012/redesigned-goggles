#pragma once
#include "Core/IGameView.h"
#include "Systems/SDLManager.h"
#include "Core/Inventory.h"
#include "Core/Card.h"
#include "Systems/CraftingSystem.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UICard.h"
#include "Interface/ui/UITooltip.h"
#include "Interface/ui/UICraftingPanel.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include "Interface/ui/UIManager.h"

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
               const CraftingSystem& craftingSystem,
               int inventoryScrollOffset = 0,
               int craftingScrollOffset = 0) override;
    
    const Card* getHoveredCard(const Inventory& inventory, int mouseX, int mouseY, int scrollOffset = 0) const override;
    bool isPointInUIArea(int x, int y, const std::string& areaName) const override;
    int getClickedRecipeIndex(int mouseX, int mouseY, int scrollOffset = 0) const override;
    bool isButtonHovered(const std::string& buttonName, int mouseX, int mouseY) const override;
    bool isCraftingPanelHovered(int mouseX, int mouseY) const override;

private:
    SDLManager& sdlManager_;
    
    // UI Framework
    UIManager uiManager_;

    // Components stored as shared pointers so UIManager can own references
    std::vector<std::shared_ptr<UIButton>> buttons_;
    std::vector<std::shared_ptr<UICard>> cards_;
    std::shared_ptr<UITooltip> tooltip_;
    std::shared_ptr<UICraftingPanel> craftingPanel_;
    
    // UI area definitions for hit testing
    std::unordered_map<std::string, SDL_Rect> uiAreas_;
    
    // Component management - pure rendering logic
    void createButtons();
    void updateCards(const Inventory& inventory, int scrollOffset = 0);
    void renderBackground();
    void renderInventoryBackground();
    void renderHints();
    void updateTooltip(const Inventory& inventory, 
                      const Card* selectedCard, 
                      bool showCraftingPanel, 
                      int mouseX, 
                      int mouseY,
                      int scrollOffset = 0);
    
    // Scroll-aware rendering helpers
    void renderScrollableCardList(const Inventory& inventory, int scrollOffset);
    void renderScrollIndicators(const Inventory& inventory, int inventoryScrollOffset, 
                               int craftingScrollOffset, bool showCraftingPanel, 
                               const CraftingSystem& craftingSystem);
    
    // UI state helpers - no business logic
    void initializeUIAreas();
    SDL_Rect getButtonRect(const std::string& buttonName) const;
    SDL_Rect getCraftingPanelRect() const;
};