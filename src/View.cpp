#include "View.h"
#include "Constants.h"
#include <iostream>

View::View(SDLManager& sdl) 
    : sdlManager_(sdl) {
    
    // Create UI components
    createButtons();
    tooltip_ = std::make_unique<UITooltip>(sdlManager_);
    
    // Initialize crafting panel (callback will be set later)
    craftingPanel_ = std::make_unique<UICraftingPanel>(sdlManager_);
}

void View::render(const Inventory& inventory, const Card* selectedCard, int mouseX, int mouseY, 
                           bool showCraftingPanel, const CraftingSystem& craftingSystem) {
    
    renderBackground();
    
    // Update and render cards
    updateCards(inventory);
    for (auto& card : cards_) {
        card->render();
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
    for (auto& button : buttons_) {
        button->render();
    }
    
    // Handle crafting panel
    if (showCraftingPanel) {
        craftingPanel_->show();
        craftingPanel_->update(craftingSystem, inventory);
    } else {
        craftingPanel_->hide();
    }
    craftingPanel_->render();
    
    // Render hints
    renderHints();
    
    // Handle tooltip
    handleTooltip(inventory, selectedCard, showCraftingPanel, mouseX, mouseY);
    tooltip_->render();
    
    SDL_RenderPresent(sdlManager_.getRenderer());
}

void View::handleClick(int mouseX, int mouseY) {
    // Handle button clicks
    for (auto& button : buttons_) {
        button->handleClick(mouseX, mouseY);
    }
    
    // Handle crafting panel clicks
    craftingPanel_->handleClick(mouseX, mouseY);
}

const Card* View::getHoveredCard(const Inventory& inventory, int mouseX, int mouseY) {
    for (auto& uiCard : cards_) {
        if (uiCard->isPointInside(mouseX, mouseY)) {
            return &uiCard->getCard();
        }
    }
    return nullptr;
}

void View::setButtonCallbacks(
    std::function<void()> onAddCard,
    std::function<void()> onRemoveCard,
    std::function<void()> onExplore,
    std::function<void()> onCraft) {
    
    onAddCard_ = onAddCard;
    onRemoveCard_ = onRemoveCard;
    onExplore_ = onExplore;
    onCraft_ = onCraft;
    
    // Update button callbacks
    if (buttons_.size() >= 4) {
        buttons_[0]->setOnClick(onAddCard_);
        buttons_[1]->setOnClick(onRemoveCard_);
        buttons_[2]->setOnClick(onExplore_);
        buttons_[3]->setOnClick(onCraft_);
    }
}

void View::setCraftingCallback(std::function<void(const Recipe&)> onRecipeCraft) {
    if (craftingPanel_) {
        craftingPanel_ = std::make_unique<UICraftingPanel>(sdlManager_, onRecipeCraft);
    }
}

void View::createButtons() {
    buttons_.clear();
    
    // Create buttons with their text and positions
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

void View::updateCards(const Inventory& inventory) {
    cards_.clear();
    
    int index = 0;
    for (const auto& card : inventory.getCards()) {
        int cardY = Constants::CARD_X + index * Constants::CARD_SPACING;
        
        auto uiCard = std::make_unique<UICard>(card, Constants::CARD_X, cardY, sdlManager_);
        cards_.push_back(std::move(uiCard));
        
        index++;
    }
}

void View::renderBackground() {
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_SetRenderDrawColor(renderer, Constants::BACKGROUND_COLOR.r, Constants::BACKGROUND_COLOR.g,
                           Constants::BACKGROUND_COLOR.b, Constants::BACKGROUND_COLOR.a);
    SDL_RenderClear(renderer);
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

void View::handleTooltip(const Inventory& inventory, const Card* selectedCard, 
                                  bool showCraftingPanel, int mouseX, int mouseY) {
    // Show tooltip only when not dragging and crafting panel is not shown
    if (!selectedCard && !showCraftingPanel) {
        const Card* hoveredCard = getHoveredCard(inventory, mouseX, mouseY);
        if (hoveredCard) {
            tooltip_->showForCard(*hoveredCard, mouseX, mouseY);
        } else {
            tooltip_->hide();
        }
    } else {
        tooltip_->hide();
    }
}
