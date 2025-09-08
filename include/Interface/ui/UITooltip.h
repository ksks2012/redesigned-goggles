#pragma once
#include "UIComponent.h"
#include "TooltipData.h"
#include <vector>

// Forward declaration to avoid including Card.h in the UI library
class Card;

/**
 * Tooltip UI component for displaying detailed item information
 * Now uses generic TooltipData instead of being coupled to Card
 */
class UITooltip : public UIComponent {
public:
    UITooltip(SDLManager& sdlManager);
    
    void layout() override;
    void render() override;
    
    // Generic method for showing tooltip with any data
    void show(const TooltipData& data, int mouseX, int mouseY);
    
    // Convenience method for ITooltipProvider objects
    void showForProvider(const ITooltipProvider& provider, int mouseX, int mouseY);
    
    void hide();
    
    bool isVisible() const { return visible_; }

private:
    std::vector<std::string> tooltipLines_;
    bool visible_;
    int mouseX_, mouseY_;
    
    // Convert TooltipData to display lines
    void generateDisplayLines(const TooltipData& data);
        
    void calculateOptimalPosition();
    void calculateSize();
};