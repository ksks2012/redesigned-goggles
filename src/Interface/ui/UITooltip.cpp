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

void UITooltip::show(const TooltipData& data, int mouseX, int mouseY) {
    mouseX_ = mouseX;
    mouseY_ = mouseY;
    
    generateDisplayLines(data);
    calculateSize();
    calculateOptimalPosition();
    
    visible_ = true;
}

void UITooltip::showForProvider(const ITooltipProvider& provider, int mouseX, int mouseY) {
    show(provider.getTooltipData(), mouseX, mouseY);
}

void UITooltip::hide() {
    visible_ = false;
}

void UITooltip::generateDisplayLines(const TooltipData& data) {
    tooltipLines_.clear();
    
    if (data.isEmpty()) {
        return;
    }
    
    // Add title
    if (!data.title.empty()) {
        if (!data.subtitle.empty()) {
            tooltipLines_.push_back(data.title + " (" + data.subtitle + ")");
        } else {
            tooltipLines_.push_back(data.title);
        }
    }
    
    // Add attributes
    for (const auto& attr : data.attributes) {
        std::string line = attr.name + ": " + attr.value;
        if (!attr.unit.empty()) {
            line += attr.unit;
        }
        tooltipLines_.push_back(line);
    }
    
    // Add tags
    for (const auto& tag : data.tags) {
        tooltipLines_.push_back(tag.tag);
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
