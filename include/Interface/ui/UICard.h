#pragma once
#include "UIComponent.h"
#include "CardDisplayData.h"

// Forward declaration to avoid including Card.h in the UI library
class Card;

/**
 * Generic card UI component for displaying any item as a card.
 * Now uses CardDisplayData instead of being coupled to Card class.
 * Supports selection highlighting and drag rendering.
 */
class UICard : public UIComponent {
public:
    // Generic constructor using CardDisplayData
    UICard(const CardDisplayData& data, int x, int y, SDLManager& sdlManager);
    
    // Constructor for ICardDisplayProvider objects
    UICard(const ICardDisplayProvider& provider, int x, int y, SDLManager& sdlManager);
    
    // Legacy constructor for backwards compatibility with Card objects
    UICard(const Card& card, int x, int y, SDLManager& sdlManager);
    
    void render() override;
    void renderDragging(int mouseX, int mouseY);
    void handleEvent(const SDL_Event& event) override;
    
    // Generic data management
    void setDisplayData(const CardDisplayData& data);
    void setFromProvider(const ICardDisplayProvider& provider);
    const CardDisplayData& getDisplayData() const { return displayData_; }
    
    // Legacy methods for backwards compatibility
    void setCard(const Card& card);
    const Card* getCard() const { return legacyCard_; }
    const Card& getCardRef() const; // Returns reference, throws if no legacy card
    
    // Visual state
    void setSelected(bool selected);
    bool isSelected() const { return selected_; }
    
    // Utility - now works with generic display data
    bool compareDisplayData(const CardDisplayData& other) const;
    bool compareCard(const Card& other) const; // Legacy method

private:
    CardDisplayData displayData_;
    bool selected_;
    
    // Legacy support - pointer to original Card object if available
    const Card* legacyCard_;
    
    SDL_Color getRarityColor() const;
    SDL_Color getBackgroundColor() const;
    SDL_Color getTextColor() const;
    std::string getDisplayText() const;
};