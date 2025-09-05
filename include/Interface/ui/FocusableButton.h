#pragma once
#include "Constants.h"
#include "Interface/ui/UIComponent.h"
#include <functional>

/**
 * A simple focusable button component that demonstrates focus handling
 * Shows different visual states for normal, hovered, focused, and pressed states
 */
class FocusableButton : public UIComponent {
public:
    FocusableButton(int x, int y, int width, int height, SDLManager& sdlManager, 
                   const std::string& text, std::function<void()> onClick = nullptr);
    
    // UIComponent overrides
    void render() override;
    void handleEvent(const SDL_Event& event) override;
    
    // Focus handling
    void onFocusGained() override;
    void onFocusLost() override;
    bool canReceiveFocus() const override { return true; }
    
    // Button properties
    void setText(const std::string& text) { text_ = text; }
    const std::string& getText() const { return text_; }
    void setOnClick(std::function<void()> onClick) { onClick_ = onClick; }

private:
    std::string text_;
    std::function<void()> onClick_;
    
    // State tracking
    bool isHovered_ = false;
    bool isPressed_ = false;
        
    void updateHoverState(int mouseX, int mouseY);
};
