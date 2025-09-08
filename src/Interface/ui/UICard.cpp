#include "Interface/ui/UICard.h"
#include "Core/Card.h"  // Only needed for legacy Card support
#include "Systems/SDLManager.h"
#include <iostream>
#include <stdexcept>

UICard::UICard(const CardDisplayData& data, int x, int y, SDLManager& sdlManager)
    : UIComponent(x, y, Constants::CARD_WIDTH, Constants::CARD_HEIGHT, sdlManager),
      displayData_(data),
      selected_(false),
      legacyCard_(nullptr) {
}

UICard::UICard(const ICardDisplayProvider& provider, int x, int y, SDLManager& sdlManager)
    : UICard(provider.getCardDisplayData(), x, y, sdlManager) {
}

UICard::UICard(const Card& card, int x, int y, SDLManager& sdlManager)
    : UIComponent(x, y, Constants::CARD_WIDTH, Constants::CARD_HEIGHT, sdlManager),
      selected_(false),
      legacyCard_(&card) {
    // Convert Card to CardDisplayData for internal use
    displayData_.name = card.name;
    displayData_.type = card.getTypeString();
    displayData_.quantity = card.quantity;
    displayData_.rarity = card.rarity;
}

void UICard::render() {
    // Render card background with appropriate color
    SDL_Color bgColor = getBackgroundColor();
    renderBackground(bgColor);

    // Render border (red highlight if selected, normal border otherwise)
    SDL_Color borderColor = selected_ ? Constants::SELECTED_BORDER_COLOR : Constants::BORDER_COLOR;
    renderBorder(borderColor, selected_ ? 3 : 1);

    // Render card text
    std::string displayText = getDisplayText();
    SDL_Color textColor = getTextColor();
    renderText(displayText, Constants::CARD_TEXT_OFFSET_X, Constants::CARD_TEXT_OFFSET_Y, textColor);
}

void UICard::renderDragging(int mouseX, int mouseY) {
    // Temporarily change position for dragging
    int originalX = x_;
    int originalY = y_;
    
    setPosition(mouseX + Constants::DRAG_CARD_OFFSET_X, mouseY + Constants::DRAG_CARD_OFFSET_Y);
    
    // Render with slightly transparent effect
    SDL_Color bgColor = getBackgroundColor();
    bgColor.a = 200; // Make it slightly transparent
    renderBackground(bgColor);
    renderBorder(Constants::TEXT_COLOR);
    
    // Render text with offset for dragging
    std::string displayText = getDisplayText();
    SDL_Color textColor = getTextColor();
    renderText(displayText, Constants::DRAG_TEXT_OFFSET_X - Constants::DRAG_CARD_OFFSET_X, 
               Constants::DRAG_TEXT_OFFSET_Y - Constants::DRAG_CARD_OFFSET_Y, textColor);
    
    // Restore original position
    setPosition(originalX, originalY);
}

void UICard::handleEvent(const SDL_Event& event) {
    // Simple card event handling
    // Card selection is managed by parent container
}

void UICard::setDisplayData(const CardDisplayData& data) {
    displayData_ = data;
    legacyCard_ = nullptr; // Clear legacy card reference
}

void UICard::setFromProvider(const ICardDisplayProvider& provider) {
    setDisplayData(provider.getCardDisplayData());
}

void UICard::setCard(const Card& card) {
    displayData_.name = card.name;
    displayData_.type = card.getTypeString();
    displayData_.quantity = card.quantity;
    displayData_.rarity = card.rarity;
    displayData_.clearCustomColors(); // Use rarity-based colors
    legacyCard_ = &card;
}

const Card& UICard::getCardRef() const {
    if (!legacyCard_) {
        throw std::runtime_error("No legacy Card object available - UICard was not created from Card");
    }
    return *legacyCard_;
}

void UICard::setSelected(bool selected) {
    selected_ = selected;
}

bool UICard::compareDisplayData(const CardDisplayData& other) const {
    return (displayData_.name == other.name &&
            displayData_.rarity == other.rarity &&
            displayData_.quantity == other.quantity &&
            displayData_.type == other.type);
}

bool UICard::compareCard(const Card& other) const {
    return (displayData_.name == other.name &&
            displayData_.rarity == other.rarity &&
            displayData_.quantity == other.quantity &&
            displayData_.type == other.getTypeString());
}

SDL_Color UICard::getRarityColor() const {
    switch (displayData_.rarity) {
        case 1: return Constants::RARITY_COMMON;
        case 2: return Constants::RARITY_RARE;
        case 3: return Constants::RARITY_LEGENDARY;
        default: return Constants::RARITY_COMMON;
    }
}

SDL_Color UICard::getBackgroundColor() const {
    if (displayData_.useCustomColors) {
        return displayData_.backgroundColor;
    }
    return getRarityColor();
}

SDL_Color UICard::getTextColor() const {
    if (displayData_.useCustomColors) {
        return displayData_.textColor;
    }
    return Constants::TEXT_COLOR;
}

std::string UICard::getDisplayText() const {
    return displayData_.getFormattedDisplayText();
}
