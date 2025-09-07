#include "Interface/ui/UIInventoryContainer.h"
#include "Interface/ui/UICard.h"
#include "Systems/SDLManager.h"
#include "Constants.h"
#include <algorithm>

UIInventoryContainer::UIInventoryContainer(int x, int y, int width, int height, SDLManager& sdlManager)
    : UIContainer(x, y, width, height, sdlManager) {
    // Pre-allocate some cards in the pool to avoid frequent allocations
    cardPool_.reserve(20);  // Reasonable default for most inventories
}

void UIInventoryContainer::updateInventory(const Inventory& inventory) {
    // Save selection state before clearing
    saveSelectionState();
    
    // Update inventory data
    inventoryCards_ = inventory.getCards();
    lastInventorySize_ = inventoryCards_.size();
    
    // Reset pool for reuse
    resetPool();
    
    // Calculate visible range and create only necessary cards
    int startIndex, endIndex;
    calculateVisibleRange(startIndex, endIndex);
    
    // Create/update only visible cards
    for (int i = startIndex; i < endIndex; ++i) {
        if (i < 0 || i >= static_cast<int>(inventoryCards_.size())) {
            continue;
        }
        
        const Card& card = inventoryCards_[i];
        int cardY = getCardYPosition(i);
        
        if (isCardVisible(cardY)) {
            UICard* uiCard = getCardFromPool();
            if (uiCard) {
                int cardX = x_ + Constants::INVENTORY_MARGIN;
                uiCard->setCard(card);
                uiCard->setPosition(cardX, cardY);
                restoreSelectionState(uiCard, card);
            }
        }
    }
}
#include <iostream>
void UIInventoryContainer::updateScroll(int scrollOffset) {
    // Check if we need to initialize inventory data first
    if (inventoryCards_.empty()) {
        return;  // No inventory to display
    }
    
    // Only update if scroll offset actually changed
    if (getScrollOffset() == scrollOffset) {
        return;  // No change needed
    }
    
    // Debug output
    // std::cout << "Updating scroll: " << getScrollOffset() << " -> " << scrollOffset << std::endl;
    
    // Update scroll offset
    setScrollOffset(scrollOffset);
    
    // Save current selection state
    saveSelectionState();
    
    // Reset pool for reuse
    resetPool();
    
    // Calculate new visible range and create only necessary cards
    int startIndex, endIndex;
    calculateVisibleRange(startIndex, endIndex);
    
    // Create/update only visible cards
    for (int i = startIndex; i < endIndex; ++i) {
        if (i < 0 || i >= static_cast<int>(inventoryCards_.size())) {
            continue;
        }
        
        const Card& card = inventoryCards_[i];
        UICard* uiCard = getCardFromPool();
        if (uiCard) {
            int cardX = x_ + Constants::INVENTORY_MARGIN;
            int cardY = getCardYPosition(i);
            uiCard->setCard(card);
            uiCard->setPosition(cardX, cardY);
            restoreSelectionState(uiCard, card);
        }
    }
}

// Helper method to check if inventory needs updating
bool UIInventoryContainer::needsInventoryUpdate(const Inventory& inventory) const {
    const auto& cards = inventory.getCards();
    return cards.size() != lastInventorySize_ || inventoryCards_.empty();
}
#include <iostream>
void UIInventoryContainer::render() {
    // Set clipping to container bounds
    SDL_Rect clip = {x_, y_, width_, height_};
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_RenderSetClipRect(renderer, &clip);
    
    // Only render cards that are actually visible in the viewport
    for (size_t i = 0; i < usedCards_; ++i) {
        if (cardPool_[i]) {
            SDL_Rect cardRect = cardPool_[i]->getRect();
            
            // Check if card is within visible bounds (considering scroll)
            if (cardRect.y + cardRect.h >= y_ && cardRect.y <= y_ + height_) {
                cardPool_[i]->render();
            }
        }
    }
    
    SDL_RenderSetClipRect(renderer, nullptr);
}

void UIInventoryContainer::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mouseX = event.button.x;
        int mouseY = event.button.y;
        
        const Card* clickedCard = getCardAtPosition(mouseX, mouseY);
        if (clickedCard && onCardClick_) {
            onCardClick_(*clickedCard);
        }
    }
    
    // Forward event to active cards
    for (size_t i = 0; i < usedCards_; ++i) {
        if (cardPool_[i]) {
            cardPool_[i]->handleEvent(event);
        }
    }
}

const Card* UIInventoryContainer::getCardAtPosition(int x, int y) const {
    return getCardAtPosition(x, y, getScrollOffset());
}

const Card* UIInventoryContainer::getCardAtPosition(int x, int y, int scrollOffset) const {
    // Check if position is within container bounds
    if (x < x_ || x >= x_ + width_ || y < y_ || y >= y_ + height_) {
        return nullptr;
    }
    
    // Calculate which card index based on Y position
    int relativeY = y - y_ + scrollOffset;
    int cardIndex = (relativeY - Constants::INVENTORY_MARGIN) / Constants::CARD_SPACING;
    
    if (cardIndex >= 0 && cardIndex < static_cast<int>(inventoryCards_.size())) {
        // Check X bounds too
        int cardX = x_ + Constants::INVENTORY_MARGIN;
        if (x >= cardX && x <= cardX + Constants::CARD_WIDTH) {
            return &inventoryCards_[cardIndex];
        }
    }
    
    return nullptr;
}

void UIInventoryContainer::setSelectedCard(const Card* card) {
    // Update selection state
    if (selectedCard_) {
        selectionState_[getCardKey(*selectedCard_)] = false;
    }
    
    selectedCard_ = card;
    
    if (selectedCard_) {
        selectionState_[getCardKey(*selectedCard_)] = true;
    }
    
    // Update visual state of active cards
    for (size_t i = 0; i < usedCards_; ++i) {
        if (cardPool_[i]) {
            bool isSelected = selectedCard_ && cardPool_[i]->compareCard(*selectedCard_);
            cardPool_[i]->setSelected(isSelected);
        }
    }
}

void UIInventoryContainer::calculateVisibleRange(int& startIndex, int& endIndex) const {
    int visibleCards = height_ / Constants::CARD_SPACING;
    startIndex = std::max(0, (getScrollOffset() / Constants::CARD_SPACING) - BUFFER_CARDS);
    endIndex = std::min(static_cast<int>(inventoryCards_.size()), 
                       startIndex + visibleCards + (2 * BUFFER_CARDS));
}

UICard* UIInventoryContainer::getCardFromPool() {
    // Expand pool if needed
    if (usedCards_ >= cardPool_.size()) {
        int cardX = x_ + Constants::INVENTORY_MARGIN;
        int cardY = y_ + Constants::INVENTORY_MARGIN; // Temporary position
        
        cardPool_.push_back(std::make_unique<UICard>(
            Card("", 1, CardType::MISC, 1), cardX, cardY, sdlManager_
        ));
    }
    
    return cardPool_[usedCards_++].get();
}

void UIInventoryContainer::resetPool() {
    usedCards_ = 0;
}

std::string UIInventoryContainer::getCardKey(const Card& card) const {
    return card.name + "_" + std::to_string(card.rarity);
}

void UIInventoryContainer::saveSelectionState() {
    selectionState_.clear();
    
    for (size_t i = 0; i < usedCards_; ++i) {
        if (cardPool_[i]) {
            const Card& card = cardPool_[i]->getCard();
            selectionState_[getCardKey(card)] = cardPool_[i]->isSelected();
        }
    }
}

void UIInventoryContainer::restoreSelectionState(UICard* uiCard, const Card& card) {
    std::string key = getCardKey(card);
    auto it = selectionState_.find(key);
    if (it != selectionState_.end()) {
        uiCard->setSelected(it->second);
    }
}

int UIInventoryContainer::getCardYPosition(int cardIndex) const {
    return y_ + Constants::INVENTORY_MARGIN + cardIndex * Constants::CARD_SPACING - getScrollOffset();
}

bool UIInventoryContainer::isCardVisible(int cardY) const {
    return cardY + Constants::CARD_HEIGHT >= y_ - Constants::CARD_SPACING && 
           cardY <= y_ + height_ + Constants::CARD_SPACING;
}

int UIInventoryContainer::getMaxScroll() const {
    // Calculate max scroll based on inventory cards
    if (inventoryCards_.empty()) {
        return 0;
    }
    
    // Total height needed = (number of cards * card spacing) + top/bottom margins
    int totalContentHeight = inventoryCards_.size() * Constants::CARD_SPACING + 2 * Constants::INVENTORY_MARGIN;
    
    // Max scroll = total content height - visible container height
    return std::max(0, totalContentHeight - height_);
}
