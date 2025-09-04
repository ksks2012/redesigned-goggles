#pragma once
#include "UIContainer.h"
#include "Core/Card.h"

/**
 * Card UI component for displaying inventory cards
 * Inherits from UIContainer to support scroll management and layout management
 */
class UICard : public UIContainer {
public:
    UICard(const Card& card, int x, int y, SDLManager& sdlManager);
    
    void render() override;
    void renderDragging(int mouseX, int mouseY);
    void handleEvent(const SDL_Event& event) override;
    
    // Card management
    void setCard(const Card& card);
    const Card& getCard() const { return card_; }
    
    // Visual state
    void setSelected(bool selected);
    bool isSelected() const { return selected_; }
    
    bool compareCard(const Card& other) const;

    // Layout management for card content
    void updateLayout();

private:
    Card card_;
    bool selected_;
    
    SDL_Color getRarityColor() const;
    std::string getDisplayText() const;
    void createCardContent();  // Create child components for card content
};