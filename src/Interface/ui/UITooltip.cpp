#include "Interface/ui/UITooltip.h"
#include "utils.h"

UITooltip::UITooltip(SDLManager& sdlManager)
    : UIComponent(0, 0, 0, 0, sdlManager),
      visible_(false),
      mouseX_(0),
      mouseY_(0) {
}

void UITooltip::layout() {
    // Recalculate size and position if tooltip is visible
    // This separates layout calculation from rendering
    if (visible_ && !tooltipLines_.empty()) {
        calculateSize();
        calculateOptimalPosition();
    }
}

void UITooltip::render() {
    if (!visible_ || tooltipLines_.empty()) {
        return;
    }
    
    // Render background
    renderBackground(Constants::TOOLTIP_BG_COLOR);
    
    // Render border
    renderBorder(Constants::ATTRIBUTE_TEXT_COLOR);
    
    // Render text lines
    for (size_t i = 0; i < tooltipLines_.size(); ++i) {
        SDL_Color textColor = (i == 0) ? Constants::TEXT_COLOR : Constants::ATTRIBUTE_TEXT_COLOR;
        renderText(tooltipLines_[i], Constants::TOOLTIP_PADDING, 
                   Constants::TOOLTIP_PADDING + i * Constants::TOOLTIP_LINE_HEIGHT, textColor);
    }
}

void UITooltip::showForCard(const Card& card, int mouseX, int mouseY) {
    mouseX_ = mouseX;
    mouseY_ = mouseY;
    
    generateCardInfo(card);
    calculateSize();
    calculateOptimalPosition();
    
    visible_ = true;
}

void UITooltip::hide() {
    visible_ = false;
}

void UITooltip::generateCardInfo(const Card& card) {
    tooltipLines_.clear();

    // Title: name and type
    tooltipLines_.push_back(card.name + " (" + card.getTypeString() + ")");

    // Basic info
    tooltipLines_.push_back(Constants::TOOLTIP_RARITY + ": " + std::to_string(card.rarity) +
                           (card.rarity == 1 ? " (" + Constants::TOOLTIP_COMMON + ")" :
                            card.rarity == 2 ? " (" + Constants::TOOLTIP_RARE + ")" : " (" + Constants::TOOLTIP_LEGENDARY + ")"));

    tooltipLines_.push_back(Constants::TOOLTIP_QUANTITY + ": " + std::to_string(card.quantity));

    // Attribute info
    if (card.hasAttribute(AttributeType::WEIGHT)) {
        tooltipLines_.push_back(Constants::TOOLTIP_WEIGHT + ": " + to_string_with_precision(card.getAttribute(AttributeType::WEIGHT), 2) + "kg");
        if (card.quantity > 1) {
            tooltipLines_.push_back(Constants::TOOLTIP_TOTAL_WEIGHT + ": " + to_string_with_precision(card.getTotalWeight(), 2) + "kg");
        }
    }

    if (card.hasAttribute(AttributeType::NUTRITION)) {
        tooltipLines_.push_back(Constants::TOOLTIP_NUTRITION + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::NUTRITION))));
    }

    if (card.hasAttribute(AttributeType::ATTACK)) {
        tooltipLines_.push_back(Constants::TOOLTIP_ATTACK + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::ATTACK))));
    }

    if (card.hasAttribute(AttributeType::DEFENSE)) {
        tooltipLines_.push_back(Constants::TOOLTIP_DEFENSE + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::DEFENSE))));
    }

    if (card.hasAttribute(AttributeType::HEALING)) {
        tooltipLines_.push_back(Constants::TOOLTIP_HEALING + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::HEALING))));
    }

    if (card.hasAttribute(AttributeType::DURABILITY)) {
        tooltipLines_.push_back(Constants::TOOLTIP_DURABILITY + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::DURABILITY))));
    }

    if (card.hasAttribute(AttributeType::BURN_VALUE)) {
        tooltipLines_.push_back(Constants::TOOLTIP_BURN_VALUE + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::BURN_VALUE))));
    }

    if (card.hasAttribute(AttributeType::CRAFTING_VALUE)) {
        tooltipLines_.push_back(Constants::TOOLTIP_CRAFTING_VALUE + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::CRAFTING_VALUE))));
    }

    if (card.hasAttribute(AttributeType::TRADE_VALUE)) {
        tooltipLines_.push_back(Constants::TOOLTIP_TRADE_VALUE + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::TRADE_VALUE))));
    }

    // Functional info
    if (card.isEdible()) {
        tooltipLines_.push_back(Constants::TOOLTIP_EDIBLE);
    }

    if (card.isBurnable()) {
        tooltipLines_.push_back(Constants::TOOLTIP_BURNABLE);
    }
}

void UITooltip::calculateOptimalPosition() {
    // Start with mouse position plus offset
    int tooltipX = mouseX_ + Constants::TOOLTIP_MOUSE_OFFSET;
    int tooltipY = mouseY_ - height_ / 2;
    
    // Adjust to avoid screen edges
    if (tooltipX + width_ > Constants::WINDOW_WIDTH) {
        tooltipX = mouseX_ - width_ - Constants::TOOLTIP_MOUSE_OFFSET;
    }
    if (tooltipY < 0) {
        tooltipY = Constants::TOOLTIP_SCREEN_MARGIN;
    }
    if (tooltipY + height_ > Constants::WINDOW_HEIGHT) {
        tooltipY = Constants::WINDOW_HEIGHT - height_ - Constants::TOOLTIP_SCREEN_MARGIN;
    }
    
    setPosition(tooltipX, tooltipY);
}

void UITooltip::calculateSize() {
    int maxWidth = 0;
    
    for (const auto& line : tooltipLines_) {
        int textW, textH;
        getTextSize(line, textW, textH);
        maxWidth = std::max(maxWidth, textW);
    }
    
    width_ = maxWidth + 2 * Constants::TOOLTIP_PADDING;
    height_ = tooltipLines_.size() * Constants::TOOLTIP_LINE_HEIGHT + 2 * Constants::TOOLTIP_PADDING;
}
