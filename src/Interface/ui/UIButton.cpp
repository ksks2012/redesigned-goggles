#include "Interface/ui/UIButton.h"

UIButton::UIButton(const std::string& text, int x, int y, int minWidth, int height, 
                   SDLManager& sdlManager, std::function<void()> onClick)
    : UIComponent(x, y, minWidth, height, sdlManager),
      text_(text),
      onClick_(onClick),
      enabled_(true),
      backgroundColor_(Constants::BUTTON_COLOR),
      textColor_(Constants::TEXT_COLOR),
      borderColor_(Constants::BORDER_COLOR),
      minWidth_(minWidth),
      textPadding_(Constants::BUTTON_TEXT_PADDING) {
    updateSize();
}

void UIButton::layout() {
    // Calculate button size based on text content
    // This separates size calculation from rendering
    updateSize();
}

void UIButton::render() {
    // Render background
    SDL_Color bgColor = enabled_ ? backgroundColor_ : Constants::RECIPE_DISABLED_TEXT;
    renderBackground(bgColor);
    
    // Render border
    renderBorder(borderColor_);
    
    // Render text (centered)
    int textW, textH;
    getTextSize(text_, textW, textH);
    int textX = (width_ - textW) / 2;
    int textY = (height_ - textH) / 2;
    
    SDL_Color textCol = enabled_ ? textColor_ : Constants::BORDER_COLOR;
    renderText(text_, textX, textY, textCol);
}

void UIButton::handleClick(int mouseX, int mouseY) {
    if (enabled_ && isPointInside(mouseX, mouseY) && onClick_) {
        onClick_();
    }
}

void UIButton::setText(const std::string& text) {
    text_ = text;
    updateSize();
}

void UIButton::setColors(SDL_Color background, SDL_Color text, SDL_Color border) {
    backgroundColor_ = background;
    textColor_ = text;
    borderColor_ = border;
}

void UIButton::setOnClick(std::function<void()> onClick) {
    onClick_ = onClick;
}

void UIButton::setEnabled(bool enabled) {
    enabled_ = enabled;
}

void UIButton::updateSize() {
    int textW, textH;
    getTextSize(text_, textW, textH);
    width_ = std::max(minWidth_, textW + 2 * textPadding_);
}
