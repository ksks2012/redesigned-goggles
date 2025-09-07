#pragma once
#include "UIComponent.h"
#include "Core/Card.h"

/**
 * Simple card UI component for displaying individual inventory cards.
 * No longer inherits from UIContainer - just a lightweight display component.
 * Supports selection highlighting and drag rendering.
 */
class UICard : public UIComponent {
public:
    UICard(const Card& card, int x, int y, SDLManager& sdlManager);
    
    void render() override;
    void renderDragging(int mouseX, int mouseY);
    void handleEvent(const SDL_Event& event) override;
    
    // Card data management - lightweight updates without recreation
    void setCard(const Card& card);
    const Card& getCard() const { return card_; }
    
    // Visual state
    void setSelected(bool selected);
    bool isSelected() const { return selected_; }
    
    // Utility
    bool compareCard(const Card& other) const;

private:
    Card card_;
    bool selected_;
    
    SDL_Color getRarityColor() const;
    std::string getDisplayText() const;
};