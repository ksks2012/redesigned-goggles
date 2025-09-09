#include "Interface/ui/UICard.h"
#include "Systems/SDLManager.h"
#include <iostream>

UICard::UICard(const CardDisplayData& data, int x, int y, SDLManager& sdlManager)
    : UIComponent(x, y, Constants::CARD_WIDTH, Constants::CARD_HEIGHT, sdlManager),
      displayData_(data),
      selected_(false) {
}

UICard::UICard(const ICardDisplayProvider& provider, int x, int y, SDLManager& sdlManager)
    : UICard(provider.getCardDisplayData(), x, y, sdlManager) {
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
}

void UICard::setFromProvider(const ICardDisplayProvider& provider) {
    setDisplayData(provider.getCardDisplayData());
}

void UICard::setSelected(bool selected) {
    selected_ = selected;
}

bool UICard::compareDisplayData(const CardDisplayData& other) const {
    return displayData_ == other;
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
