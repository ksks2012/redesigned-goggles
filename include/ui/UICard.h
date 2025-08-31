#pragma once
#include "UIComponent.h"
#include "Card.h"

/**
 * Card UI component for displaying inventory cards
 */
class UICard : public UIComponent {
public:
    UICard(const Card& card, int x, int y, SDLManager& sdlManager);
    
    void render() override;
    void renderDragging(int mouseX, int mouseY);
    
    // Card management
    void setCard(const Card& card);
    const Card& getCard() const { return card_; }
    
    // Visual state
    void setSelected(bool selected);
    bool isSelected() const { return selected_; }

private:
    Card card_;
    bool selected_;
    
    SDL_Color getRarityColor() const;
    std::string getDisplayText() const;
};