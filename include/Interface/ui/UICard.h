#pragma once
#include "UIComponent.h"
#include "CardDisplayData.h"

/**
 * Generic card UI component for displaying any item as a card.
 * Uses CardDisplayData for complete decoupling from game logic.
 * Supports selection highlighting and drag rendering.
 */
class UICard : public UIComponent {
public:
    // Generic constructor using CardDisplayData
    UICard(const CardDisplayData& data, int x, int y, SDLManager& sdlManager);
    
    // Constructor for ICardDisplayProvider objects
    UICard(const ICardDisplayProvider& provider, int x, int y, SDLManager& sdlManager);
    
    void render() override;
    void renderDragging(int mouseX, int mouseY);
    void handleEvent(const SDL_Event& event) override;
    
    // Generic data management
    void setDisplayData(const CardDisplayData& data);
    void setFromProvider(const ICardDisplayProvider& provider);
    const CardDisplayData& getDisplayData() const { return displayData_; }
    
    // Visual state
    void setSelected(bool selected);
    bool isSelected() const { return selected_; }
    
    // Utility - now works with generic display data
    bool compareDisplayData(const CardDisplayData& other) const;

private:
    CardDisplayData displayData_;
    bool selected_;
    
    SDL_Color getRarityColor() const;
    SDL_Color getBackgroundColor() const;
    SDL_Color getTextColor() const;
    std::string getDisplayText() const;
};