#pragma once
#include "UIComponent.h"
#include "Core/Card.h"
#include <vector>

/**
 * Tooltip UI component for displaying detailed card information
 */
class UITooltip : public UIComponent {
public:
    UITooltip(SDLManager& sdlManager);
    
    void layout() override;
    void render() override;
    void showForCard(const Card& card, int mouseX, int mouseY);
    void hide();
    
    bool isVisible() const { return visible_; }

private:
    std::vector<std::string> tooltipLines_;
    bool visible_;
    int mouseX_, mouseY_;
    
    void generateCardInfo(const Card& card);
    void calculateOptimalPosition();
    void calculateSize();
};