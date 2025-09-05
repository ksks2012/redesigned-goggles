#include "Interface/ui/FocusableButton.h"
#include <iostream>

FocusableButton::FocusableButton(int x, int y, int width, int height, SDLManager& sdlManager, 
                                const std::string& text, std::function<void()> onClick)
    : UIComponent(x, y, width, height, sdlManager), text_(text), onClick_(onClick) {
}

void FocusableButton::render() {
    // Choose background color based on state
    SDL_Color bgColor = Constants::NORMAL_COLOR;
    if (isPressed_) {
        bgColor = Constants::PRESSED_COLOR;
    } else if (hasFocus_) {
        bgColor = Constants::FOCUSED_COLOR;
    } else if (isHovered_) {
        bgColor = Constants::HOVERED_COLOR;
    }
    
    // Render background
    renderBackground(bgColor);
    
    // Render focus border if focused
    if (hasFocus_) {
        renderBorder(Constants::FOCUS_BORDER_COLOR, 2);
    }
    
    // Render text centered
    if (!text_.empty()) {
        int textW, textH;
        getTextSize(text_, textW, textH);
        int textX = (width_ - textW) / 2;
        int textY = (height_ - textH) / 2;
        renderText(text_, textX, textY, Constants::TEXT_COLOR);
    }
}

void FocusableButton::handleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_MOUSEMOTION:
            updateHoverState(event.motion.x, event.motion.y);
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT && isPointInside(event.button.x, event.button.y)) {
                isPressed_ = true;
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT && isPressed_) {
                isPressed_ = false;
                if (isPointInside(event.button.x, event.button.y) && onClick_) {
                    onClick_();
                }
            }
            break;
            
        case SDL_KEYDOWN:
            if (hasFocus_ && (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE)) {
                // Activate button with Enter or Space when focused
                if (onClick_) {
                    onClick_();
                }
            }
            break;
    }
}

void FocusableButton::onFocusGained() {
    std::cout << "Button '" << text_ << "' gained focus" << std::endl;
}

void FocusableButton::onFocusLost() {
    std::cout << "Button '" << text_ << "' lost focus" << std::endl;
}

void FocusableButton::updateHoverState(int mouseX, int mouseY) {
    bool wasHovered = isHovered_;
    isHovered_ = isPointInside(mouseX, mouseY);
    
    // Reset pressed state if mouse leaves while pressed
    if (wasHovered && !isHovered_ && isPressed_) {
        isPressed_ = false;
    }
}
