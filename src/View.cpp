#include "View.h"
#include "Constants.h"
#include <iostream>

View::View(SDLManager& sdl) 
    : sdlManager_(sdl) {
    
    // Create UI components and register them with the UI manager
    createButtons();
    tooltip_ = std::make_shared<UITooltip>(sdlManager_);
    craftingPanel_ = std::make_shared<UICraftingPanel>(sdlManager_);

    // Register persistent UI components
    for (auto& b : buttons_) uiManager_.addComponent(b, true);
    uiManager_.addComponent(tooltip_, true);
    uiManager_.addComponent(craftingPanel_, true);
    
    // Initialize UI areas for hit testing
    initializeUIAreas();
}

void View::render(const Inventory& inventory, const Card* selectedCard, int mouseX, int mouseY, 
                  bool showCraftingPanel, const CraftingSystem& craftingSystem,
                  int inventoryScrollOffset, int craftingScrollOffset) {
    
    renderBackground();
    
    // Render inventory area background (similar to crafting panel)
    renderInventoryBackground();
    
    // Update and render cards with scroll offset and viewport clipping
    updateCards(inventory, inventoryScrollOffset);
    
    // Set up inventory viewport clipping
    SDL_Rect inventoryViewport = {
        Constants::INVENTORY_AREA_X,
        Constants::INVENTORY_AREA_Y,
        Constants::INVENTORY_AREA_WIDTH,
        Constants::INVENTORY_AREA_HEIGHT
    };
    
    // Render cards only within inventory bounds (strict boundary checking)
    for (auto& card : cards_) {
        int cardX = card->getX();
        int cardY = card->getY();
        
        // Check if card is completely within inventory viewport
        if (cardX >= inventoryViewport.x &&
            cardX + Constants::CARD_WIDTH <= inventoryViewport.x + inventoryViewport.w &&
            cardY >= inventoryViewport.y &&
            cardY + Constants::CARD_HEIGHT <= inventoryViewport.y + inventoryViewport.h) {
            card->render();
        }
    }
    
    // Render dragged card if any
    if (selectedCard) {
        // Find the UI card corresponding to the selected card
        for (auto& uiCard : cards_) {
            if (&uiCard->getCard() == selectedCard) {
                uiCard->renderDragging(mouseX, mouseY);
                break;
            }
        }
    }
    
    // Render buttons
    // Buttons and persistent UI are rendered by UIManager
    uiManager_.renderAll();
    
    // Handle crafting panel with scroll support
    if (showCraftingPanel) {
        craftingPanel_->show();
        craftingPanel_->update(craftingSystem, inventory, craftingScrollOffset);
    } else {
        craftingPanel_->hide();
    }
    craftingPanel_->render();
    
    // Render hints
    renderHints();
    
    // Render scroll indicators
    renderScrollIndicators(inventory, inventoryScrollOffset, craftingScrollOffset, showCraftingPanel, craftingSystem);
    
    // Update and render tooltip
    updateTooltip(inventory, selectedCard, showCraftingPanel, mouseX, mouseY, inventoryScrollOffset);
    tooltip_->render();
    
    SDL_RenderPresent(sdlManager_.getRenderer());
}

const Card* View::getHoveredCard(const Inventory& inventory, int mouseX, int mouseY, int scrollOffset) const {
    // Calculate card positions directly from inventory to avoid dependency on render state
    int index = 0;
    for (const auto& card : inventory.getCards()) {
        // Use consistent coordinate system with updateCards
        int cardX = Constants::INVENTORY_AREA_X + Constants::INVENTORY_MARGIN;
        int cardY = Constants::INVENTORY_AREA_Y + Constants::INVENTORY_MARGIN + index * Constants::CARD_SPACING - scrollOffset;
        
        // Check if mouse is within card bounds
        if (mouseX >= cardX && mouseX <= cardX + Constants::CARD_WIDTH && 
            mouseY >= cardY && mouseY <= cardY + Constants::CARD_HEIGHT) {
            return &card;
        }
        
        index++;
    }
    return nullptr;
}

bool View::isPointInUIArea(int x, int y, const std::string& areaName) const {
    auto it = uiAreas_.find(areaName);
    if (it != uiAreas_.end()) {
        const SDL_Rect& rect = it->second;
        return (x >= rect.x && x < rect.x + rect.w && 
                y >= rect.y && y < rect.y + rect.h);
    }
    return false;
}

int View::getClickedRecipeIndex(int mouseX, int mouseY, int scrollOffset) const {
    if (!isCraftingPanelHovered(mouseX, mouseY)) {
        return -1;
    }
    
    int recipesStartY = Constants::CRAFT_PANEL_Y + Constants::CRAFT_PANEL_RECIPES_START_Y;
    int relativeY = mouseY - recipesStartY;
    int recipeIndex = (relativeY / Constants::RECIPE_ITEM_HEIGHT) + scrollOffset;
    return recipeIndex;
}

bool View::isButtonHovered(const std::string& buttonName, int mouseX, int mouseY) const {
    return isPointInUIArea(mouseX, mouseY, buttonName);
}

bool View::isCraftingPanelHovered(int mouseX, int mouseY) const {
    return isPointInUIArea(mouseX, mouseY, "craftingPanel");
}

void View::createButtons() {
    buttons_.clear();
    
    // Create buttons without callbacks - pure presentation
    auto addButton = std::make_unique<UIButton>(
        Constants::BUTTON_TEXT_ADD, 
        Constants::BUTTON_X, Constants::BUTTON_Y_ADD,
        Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT,
        sdlManager_
    );
    
    auto removeButton = std::make_unique<UIButton>(
        Constants::BUTTON_TEXT_REMOVE,
        Constants::BUTTON_X, Constants::BUTTON_Y_REMOVE,
        Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT,
        sdlManager_
    );
    
    auto exploreButton = std::make_unique<UIButton>(
        Constants::BUTTON_TEXT_EXPLORE,
        Constants::BUTTON_X, Constants::BUTTON_Y_EXPLORE,
        Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT,
        sdlManager_
    );
    
    auto craftButton = std::make_unique<UIButton>(
        Constants::BUTTON_TEXT_CRAFT,
        Constants::BUTTON_X, Constants::BUTTON_Y_CRAFT,
        Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT,
        sdlManager_
    );
    
    buttons_.push_back(std::move(addButton));
    buttons_.push_back(std::move(removeButton));
    buttons_.push_back(std::move(exploreButton));
    buttons_.push_back(std::move(craftButton));
}

void View::updateCards(const Inventory& inventory, int scrollOffset) {
    cards_.clear();
    
    const auto& allCards = inventory.getCards();
    
    // Calculate visible area for inventory (similar to crafting panel)
    int inventoryAreaTop = Constants::INVENTORY_AREA_Y;
    int inventoryAreaBottom = Constants::INVENTORY_AREA_Y + Constants::INVENTORY_AREA_HEIGHT;
    int visibleCards = Constants::INVENTORY_AREA_HEIGHT / Constants::CARD_SPACING;
    
    // Calculate scroll range - only render visible cards plus buffer
    int startIndex = scrollOffset / Constants::CARD_SPACING;
    int endIndex = std::min(startIndex + visibleCards + 2, static_cast<int>(allCards.size())); // +2 for buffer
    startIndex = std::max(0, startIndex - 1); // -1 for buffer
    
    // Create UI cards only for visible range
    for (int i = startIndex; i < endIndex; ++i) {
        const auto& card = allCards[i];
        // Calculate position with scroll offset applied
        // Use inventory area coordinates instead of old card constants
        int cardX = Constants::INVENTORY_AREA_X + Constants::INVENTORY_MARGIN;
        int cardY = Constants::INVENTORY_AREA_Y + Constants::INVENTORY_MARGIN + i * Constants::CARD_SPACING - scrollOffset;
        
        // Only create UI card if it's within or near the visible area
        if (cardY + Constants::CARD_HEIGHT >= inventoryAreaTop - Constants::CARD_SPACING && 
            cardY <= inventoryAreaBottom + Constants::CARD_SPACING) {
            auto uiCard = std::make_unique<UICard>(card, cardX, cardY, sdlManager_);
            cards_.push_back(std::move(uiCard));
        }
    }
}

void View::renderBackground() {
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_SetRenderDrawColor(renderer, Constants::BACKGROUND_COLOR.r, Constants::BACKGROUND_COLOR.g,
                           Constants::BACKGROUND_COLOR.b, Constants::BACKGROUND_COLOR.a);
    SDL_RenderClear(renderer);
}

void View::renderInventoryBackground() {
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    
    // Render inventory area background (similar to crafting panel)
    SDL_Rect inventoryBg = {
        Constants::INVENTORY_AREA_X - Constants::INVENTORY_MARGIN,
        Constants::INVENTORY_AREA_Y - Constants::INVENTORY_MARGIN,
        Constants::INVENTORY_AREA_WIDTH + 2 * Constants::INVENTORY_MARGIN,
        Constants::INVENTORY_AREA_HEIGHT + 2 * Constants::INVENTORY_MARGIN
    };
    
    // Background
    SDL_SetRenderDrawColor(renderer, Constants::PANEL_BG_COLOR.r, Constants::PANEL_BG_COLOR.g,
                          Constants::PANEL_BG_COLOR.b, Constants::PANEL_BG_COLOR.a);
    SDL_RenderFillRect(renderer, &inventoryBg);
    
    // Border
    SDL_SetRenderDrawColor(renderer, Constants::BORDER_COLOR.r, Constants::BORDER_COLOR.g,
                          Constants::BORDER_COLOR.b, Constants::BORDER_COLOR.a);
    SDL_RenderDrawRect(renderer, &inventoryBg);
}

void View::renderHints() {
    // Create temporary text rendering (could be further abstracted)
    int hintX = Constants::HINT_X;
    int hintY = Constants::WINDOW_HEIGHT - Constants::HINT_BOTTOM_OFFSET;
    
    // For now, create temporary UIComponent just for text rendering
    class TempTextRenderer : public UIComponent {
    public:
        TempTextRenderer(SDLManager& sdl) : UIComponent(0, 0, 0, 0, sdl) {}
        void render() override {} // Not used
        void renderTextAt(const std::string& text, int x, int y, SDL_Color color) {
            renderText(text, x, y, color);
        }
    };
    
    TempTextRenderer textRenderer(sdlManager_);
    textRenderer.renderTextAt(Constants::HINT_TITLE, hintX, hintY, Constants::TEXT_COLOR);
    textRenderer.renderTextAt(Constants::HINT_SAVE, hintX, hintY + Constants::HINT_LINE_SPACING, Constants::TEXT_COLOR);
    textRenderer.renderTextAt(Constants::HINT_LOAD, hintX, hintY + 2 * Constants::HINT_LINE_SPACING, Constants::TEXT_COLOR);
    textRenderer.renderTextAt(Constants::HINT_EXIT, hintX, hintY + 3 * Constants::HINT_LINE_SPACING, Constants::TEXT_COLOR);
}

void View::updateTooltip(const Inventory& inventory, const Card* selectedCard, 
                         bool showCraftingPanel, int mouseX, int mouseY, int scrollOffset) {
    // Show tooltip only when not dragging and crafting panel is not shown
    if (!selectedCard && !showCraftingPanel) {
        const Card* hoveredCard = getHoveredCard(inventory, mouseX, mouseY, scrollOffset);
        if (hoveredCard) {
            tooltip_->showForCard(*hoveredCard, mouseX, mouseY);
        } else {
            tooltip_->hide();
        }
    } else {
        tooltip_->hide();
    }
}

void View::initializeUIAreas() {
    // Initialize UI area rectangles for hit testing
    uiAreas_["addButton"] = {Constants::BUTTON_X, Constants::BUTTON_Y_ADD, 
                            Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT};
    
    uiAreas_["removeButton"] = {Constants::BUTTON_X, Constants::BUTTON_Y_REMOVE,
                               Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT};
    
    uiAreas_["exploreButton"] = {Constants::BUTTON_X, Constants::BUTTON_Y_EXPLORE,
                                Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT};
    
    uiAreas_["craftButton"] = {Constants::BUTTON_X, Constants::BUTTON_Y_CRAFT,
                              Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT};
    
    uiAreas_["craftingPanel"] = {Constants::CRAFT_PANEL_X, Constants::CRAFT_PANEL_Y,
                                Constants::CRAFT_PANEL_WIDTH, Constants::CRAFT_PANEL_HEIGHT};
    
    // Initialize inventory area for viewport clipping and interaction
    uiAreas_["inventoryArea"] = {Constants::INVENTORY_AREA_X, Constants::INVENTORY_AREA_Y,
                                Constants::INVENTORY_AREA_WIDTH, Constants::INVENTORY_AREA_HEIGHT};
}

SDL_Rect View::getButtonRect(const std::string& buttonName) const {
    auto it = uiAreas_.find(buttonName);
    if (it != uiAreas_.end()) {
        return it->second;
    }
    return {0, 0, 0, 0};
}

SDL_Rect View::getCraftingPanelRect() const {
    return getButtonRect("craftingPanel");
}

void View::renderScrollIndicators(const Inventory& inventory, int inventoryScrollOffset, 
                                  int craftingScrollOffset, bool showCraftingPanel, 
                                  const CraftingSystem& craftingSystem) {
    // For simplicity, just add visual feedback when scrolling is possible
    // The detailed scroll bar rendering can be added later if needed
    
    // Check if inventory needs scrolling
    int totalCards = inventory.getCards().size();
    if (totalCards > 10) { // If more than 10 cards, show scroll hint
        // Simple scroll indicator - can be enhanced later
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 100, 100, 100, 100);
        SDL_Rect scrollHint = {Constants::CARD_X + 250, Constants::CARD_X + 10, 20, 5};
        SDL_RenderFillRect(sdlManager_.getRenderer(), &scrollHint);
    }
}
