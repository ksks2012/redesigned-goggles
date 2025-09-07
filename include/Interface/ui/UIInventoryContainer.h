#pragma once
#include "UIContainer.h"
#include "Core/Inventory.h"
#include "Core/Card.h"
#include <unordered_map>
#include <functional>

// Forward declaration
class UICard;

/**
 * Specialized container for inventory virtualization.
 * Only renders visible cards to optimize performance for large inventories.
 * Manages card instances pool and handles scroll-based visibility calculation.
 */
class UIInventoryContainer : public UIContainer {
public:
    UIInventoryContainer(int x, int y, int width, int height, SDLManager& sdlManager);
    ~UIInventoryContainer() override = default;

    // Update inventory data and refresh visible cards
    void updateInventory(const Inventory& inventory);
    
    // Update scroll position and refresh visible cards (more efficient than full inventory update)
    void updateScroll(int scrollOffset);
    
    // Check if inventory content has changed and needs updating
    bool needsInventoryUpdate(const Inventory& inventory) const;
    
    // Get card at specific position (for hover detection)
    const Card* getCardAtPosition(int x, int y) const;
    const Card* getCardAtPosition(int x, int y, int scrollOffset) const;
    
    // Selection management
    void setSelectedCard(const Card* card);
    const Card* getSelectedCard() const { return selectedCard_; }
    
    // Callback for card clicks
    void setOnCardClick(std::function<void(const Card&)> callback) { onCardClick_ = callback; }

    // Override to implement virtualized rendering
    void render() override;
    void handleEvent(const SDL_Event& event) override;
    
    // Override getMaxScroll to work with inventory data instead of children
    int getMaxScroll() const override;

private:
    // Current inventory data
    std::vector<Card> inventoryCards_;
    size_t lastInventorySize_ = 0;  // Track inventory size changes
    const Card* selectedCard_ = nullptr;
    
    // Card pool for reuse - simple approach
    std::vector<std::unique_ptr<UICard>> cardPool_;
    size_t usedCards_ = 0;  // How many cards from pool are currently in use
    
    // Selection state persistence 
    std::unordered_map<std::string, bool> selectionState_;
    
    // Click callback
    std::function<void(const Card&)> onCardClick_;
    
    
    // Constants for layout - use global constants
    static constexpr int BUFFER_CARDS = 2;  // Render N extra cards above/below visible area
    
    // Helper methods
    void calculateVisibleRange(int& startIndex, int& endIndex) const;
    UICard* getCardFromPool();
    void resetPool();
    std::string getCardKey(const Card& card) const;
    void saveSelectionState();
    void restoreSelectionState(UICard* uiCard, const Card& card);
    int getCardYPosition(int cardIndex) const;
    bool isCardVisible(int cardY) const;
};
