#pragma once
#include "UIComponent.h"
#include <functional>

/**
 * Button UI component with customizable appearance and click handling
 */
class UIButton : public UIComponent {
public:
    UIButton(const std::string& text, int x, int y, int minWidth, int height, 
             SDLManager& sdlManager, std::function<void()> onClick = nullptr);
    
    void layout() override;
    void render() override;
    void handleClick(int mouseX, int mouseY);
    
    // Setters for customization
    void setText(const std::string& text);
    void setColors(SDL_Color background, SDL_Color text, SDL_Color border);
    void setOnClick(std::function<void()> onClick);
    
    // State management
    void setEnabled(bool enabled);
    bool isEnabled() const { return enabled_; }

private:
    std::string text_;
    std::function<void()> onClick_;
    bool enabled_;
    
    // Visual properties
    SDL_Color backgroundColor_;
    SDL_Color textColor_;
    SDL_Color borderColor_;
    int minWidth_;
    int textPadding_;
    
    void updateSize();
};