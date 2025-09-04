#include "Interface/ui/UICard.h"
#include <iostream>


UICard::UICard(const Card& card, int x, int y, SDLManager& sdlManager)
    : UIContainer(x, y, Constants::CARD_WIDTH, Constants::CARD_HEIGHT, sdlManager),
      card_(card),
      selected_(false) {
    createCardContent();
}

void UICard::render() {
    // Render card background with rarity color
    SDL_Color rarityColor = getRarityColor();
    renderBackground(rarityColor);

    // Render border (red highlight if selected, normal border otherwise)
    SDL_Color borderColor = selected_ ? Constants::SELECTED_BORDER_COLOR : Constants::BORDER_COLOR;
    renderBorder(borderColor, selected_ ? 3 : 1);

    // Render card text
    std::string displayText = getDisplayText();
    renderText(displayText, Constants::CARD_TEXT_OFFSET_X, Constants::CARD_TEXT_OFFSET_Y, 
               Constants::TEXT_COLOR);
}

void UICard::renderDragging(int mouseX, int mouseY) {
    // Temporarily change position for dragging
    int originalX = x_;
    int originalY = y_;
    
    setPosition(mouseX + Constants::DRAG_CARD_OFFSET_X, mouseY + Constants::DRAG_CARD_OFFSET_Y);
    
    // Render with slightly transparent effect
    SDL_Color rarityColor = getRarityColor();
    rarityColor.a = 200; // Make it slightly transparent
    renderBackground(rarityColor);
    renderBorder(Constants::TEXT_COLOR);
    
    // Render text with offset for dragging
    std::string displayText = getDisplayText();
    renderText(displayText, Constants::DRAG_TEXT_OFFSET_X - Constants::DRAG_CARD_OFFSET_X, 
               Constants::DRAG_TEXT_OFFSET_Y - Constants::DRAG_CARD_OFFSET_Y, Constants::TEXT_COLOR);
    
    // Restore original position
    setPosition(originalX, originalY);
}

void UICard::handleEvent(const SDL_Event& event) {
    // UICard no longer handles its own click events directly
    // All card selection is now managed centrally by GameInputHandler
    // This prevents duplicate event processing and ensures consistent state
    
    // Call parent event handler for any container-specific events
    UIContainer::handleEvent(event);
}

void UICard::setCard(const Card& card) {
    card_ = card;
    updateLayout();  // Update layout when card changes
}

void UICard::setSelected(bool selected) {
    selected_ = selected;
}

bool UICard::compareCard(const Card& other) const {
    return (card_.name == other.name &&
            card_.rarity == other.rarity &&
            card_.quantity == other.quantity &&
            card_.type == other.type);
}

void UICard::updateLayout() {
    // Clear existing children and recreate content
    clearChildren();
    createCardContent();
}

void UICard::createCardContent() {
    // This method creates child components for the card content
    // For now, we keep the existing rendering approach but prepare for future expansion
    // In the future, card elements like title, quantity, type could be separate UI components
}

SDL_Color UICard::getRarityColor() const {
    switch (card_.rarity) {
        case 1: return Constants::RARITY_COMMON;
        case 2: return Constants::RARITY_RARE;
        case 3: return Constants::RARITY_LEGENDARY;
        default: return Constants::RARITY_COMMON;
    }
}

std::string UICard::getDisplayText() const {
    return card_.name + " x" + std::to_string(card_.quantity) + " (" + card_.getTypeString() + ")";
}
