#include "Interface/GameInputHandler.h"
#include "Constants.h"
#include "Core/BaseManager.h"
#include <iostream>
#include <random>

GameInputHandler::GameInputHandler(IGameView& view, 
                                  Inventory& inventory, 
                                  CraftingSystem& craftingSystem,
                                  std::shared_ptr<BaseBuildingController> baseBuildingController)
    : view_(view), inventory_(inventory), craftingSystem_(craftingSystem),
      baseBuildingController_(baseBuildingController),
      running_(true), selectedCard_(nullptr), previousSelectedCard_(nullptr), showCraftingPanel_(false),
      mouseX_(0), mouseY_(0), inventoryScrollOffset_(0), craftingScrollOffset_(0),
      isDragging_(false), draggedCard_(nullptr), dragStartX_(0), dragStartY_(0) {
}

void GameInputHandler::handleMouseDown(int x, int y) {
    mouseX_ = x;
    mouseY_ = y;
    
    // Check button clicks first
    if (view_.isButtonHovered("addButton", x, y)) {
        handleButtonClick("add");
        return;
    }
    
    if (view_.isButtonHovered("removeButton", x, y)) {
        handleButtonClick("remove");
        return;
    }
    
    if (view_.isButtonHovered("exploreButton", x, y)) {
        handleButtonClick("explore");
        return;
    }
    
    if (view_.isButtonHovered("craftButton", x, y)) {
        handleButtonClick("craft");
        return;
    }
    
    // Handle crafting panel interactions
    if (showCraftingPanel_) {
        if (view_.isCraftingPanelHovered(x, y)) {
            int recipeIndex = view_.getClickedRecipeIndex(x, y, craftingScrollOffset_);
            if (recipeIndex >= 0) {
                handleRecipeClick(recipeIndex);
            }
        } else {
            // Click outside crafting panel - close it
            showCraftingPanel_ = false;
            std::cout << "Crafting panel closed (click outside)" << std::endl;
        }
        return;
    }
    
    // Card selection logic (only when crafting panel is not shown)
    if (!showCraftingPanel_) {
        const Card* hoveredCard = view_.getHoveredCard(inventory_, x, y, inventoryScrollOffset_);
        if (hoveredCard) {
            bool isSameCard = (selectedCard_ == hoveredCard);
            selectedCard_ = isSameCard ? nullptr : const_cast<Card*>(hoveredCard);
            std::cout << "Card " << (isSameCard ? "deselected: " : "selected: ") << hoveredCard->name << std::endl;

            handleCardClick(selectedCard_);
            updateUICardSelection();
            
            // Start potential drag if card is selected and buildable
            if (selectedCard_ && baseBuildingController_) {
                if (BuildingConversion::isCardBuildable(selectedCard_->name)) {
                    // Store drag start position but don't start dragging yet
                    dragStartX_ = x;
                    dragStartY_ = y;
                }
            }
            
        } else if (selectedCard_) {
            std::cout << "Card deselected (empty area clicked)" << std::endl;
            selectedCard_ = nullptr;
            updateUICardSelection();
        }
    }
}

void GameInputHandler::handleMouseUp(int x, int y) {
    mouseX_ = x;
    mouseY_ = y;
    
    // Handle drag end
    if (isDragging_) {
        endDrag(x, y);
        return;
    }
    
    // Don't automatically clear selectedCard_ on mouse up
    // Selection should persist until user clicks another card or empty area
    // This allows for proper card selection behavior and dragging in the future
    
    // Only clear if we were dragging (future drag functionality)
    // For now, maintain selection state
}

void GameInputHandler::handleMouseMotion(int x, int y) {
    mouseX_ = x;
    mouseY_ = y;
    
    // Check if we should start dragging
    if (!isDragging_ && selectedCard_ && baseBuildingController_) {
        if (BuildingConversion::isCardBuildable(selectedCard_->name) && shouldStartDrag(x, y)) {
            startDrag(selectedCard_, dragStartX_, dragStartY_);
        }
    }
    
    // Update drag position if currently dragging
    if (isDragging_) {
        updateDrag(x, y);
    }
}

void GameInputHandler::handleMouseWheel(int x, int y, int deltaY) {
    mouseX_ = x;
    mouseY_ = y;
    handleScrollWheel(x, y, deltaY);
}

void GameInputHandler::handleKeyDown(int keyCode) {
    // Get modifier states
    SDL_Keymod modifiers = SDL_GetModState();
    
    switch (keyCode) {
        case SDLK_TAB:
            // Tab to focus next, Shift+Tab to focus previous
            if (modifiers & KMOD_SHIFT) {
                requestFocusPrevious();
            } else {
                requestFocusNext();
            }
            break;
            
        case SDLK_s:
            // Press S to save
            if (saveCallback_) {
                if (saveCallback_()) {
                    std::cout << "Game saved manually!" << std::endl;
                } else {
                    std::cout << "Save failed!" << std::endl;
                }
            }
            break;

        case SDLK_l:
            // Press L to load
            if (loadCallback_) {
                if (loadCallback_()) {
                    std::cout << "Game loaded!" << std::endl;
                } else {
                    std::cout << "Load failed!" << std::endl;
                }
            }
            break;
            
        case SDLK_c:
            // Press C to toggle crafting panel
            toggleCraftingPanel();
            break;

        case SDLK_ESCAPE:
            // ESC to clear focus, close crafting panel, or exit game
            if (clearFocusCallback_) {
                clearFocusCallback_();
            }
            if (showCraftingPanel_) {
                showCraftingPanel_ = false;
                std::cout << "Crafting panel closed (ESC)" << std::endl;
            } else {
                running_ = false;
            }
            break;

        default:
            break;
    }
}

void GameInputHandler::handleQuit() {
    running_ = false;
}

void GameInputHandler::handleButtonClick(const std::string& buttonName) {
    if (buttonName == "add") {
        addRandomCard();
    } else if (buttonName == "remove") {
        removeFirstCard();
    } else if (buttonName == "explore") {
        if (exploreCallback_) {
            exploreCallback_();
        }
    } else if (buttonName == "craft") {
        toggleCraftingPanel();
    }
}

void GameInputHandler::handleCardClick(const Card* card) {
    // Card selection is handled in handleMouseDown
    // This method can be extended for card-specific actions
    // Use selectedCard_ for consistency instead of parameter
    if (selectedCard_) {
        view_.setCardSelection(selectedCard_);
    } else {
        // Handle deselection case
        view_.setCardSelection(nullptr);
    }
    
    // Update tracking for future reference
    previousSelectedCard_ = selectedCard_;
}

void GameInputHandler::handleRecipeClick(int recipeIndex) {
    auto allRecipes = craftingSystem_.getAllRecipes();
    
    if (recipeIndex >= 0 && recipeIndex < static_cast<int>(allRecipes.size())) {
        const Recipe& selectedRecipe = allRecipes[recipeIndex];
        
        if (craftingSystem_.canCraft(selectedRecipe, inventory_)) {
            craftRecipe(recipeIndex);
        } else {
            std::cout << "Cannot craft " << selectedRecipe.name 
                     << " - insufficient materials or recipe not unlocked" << std::endl;
        }
    }
}

void GameInputHandler::addRandomCard() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rarityDist(Constants::RARITY_MIN, Constants::RARITY_MAX);
    inventory_.addCard(Constants::RandomCardGenerator::generateRandomCardByRarity(rarityDist(gen)));
}

void GameInputHandler::removeFirstCard() {
    auto& cards = inventory_.getCards();
    if (!cards.empty()) {
        const Card& cardToRemove = cards[0];

        if (cardToRemove.quantity == 1) {
            // Check if the card being removed is currently selected
            if (selectedCard_ && selectedCard_->compare(cardToRemove)) {
                selectedCard_ = nullptr;  // Clear selection
            }
            
            // Check if the card being removed is currently being dragged
            if (draggedCard_ && draggedCard_->compare(cardToRemove)) {
                draggedCard_ = nullptr;
                isDragging_ = false;  // Stop dragging
            }
            
            // Check if the card being removed is the previously selected card
            if (previousSelectedCard_ && previousSelectedCard_ == &cardToRemove) {
                previousSelectedCard_ = nullptr;
            }
        }

        inventory_.removeCard(cardToRemove.name, cardToRemove.rarity);
    }
}

void GameInputHandler::toggleCraftingPanel() {
    showCraftingPanel_ = !showCraftingPanel_;
    std::cout << (showCraftingPanel_ ? "Crafting panel opened" : "Crafting panel closed") << std::endl;
}

void GameInputHandler::craftRecipe(int recipeIndex) {
    auto allRecipes = craftingSystem_.getAllRecipes();
    
    if (recipeIndex >= 0 && recipeIndex < static_cast<int>(allRecipes.size())) {
        const Recipe& recipe = allRecipes[recipeIndex];
        
        std::cout << "Attempting to craft: " << recipe.name << std::endl;
        
        CraftingResult result = craftingSystem_.craftItem(recipe, inventory_);
        
        if (result.success) {
            std::cout << "Crafting successful! Received: " << result.resultCard.name << std::endl;
        } else {
            std::cout << "Crafting failed: " << result.message << std::endl;
        }
    }
}

void GameInputHandler::handleScrollWheel(int x, int y, int deltaY) {
    const int INVENTORY_SCROLL_SPEED = 3; // Pixels to scroll per wheel notch for inventory
    const int RECIPE_SCROLL_SPEED = 1;    // Recipe items to scroll per wheel notch for crafting panel
    
    // Determine which area is being scrolled based on mouse position
    if (showCraftingPanel_ && view_.isCraftingPanelHovered(x, y)) {
        // Scroll crafting panel recipes (item-based scrolling)
        craftingScrollOffset_ -= deltaY * RECIPE_SCROLL_SPEED;
        
        // Calculate maximum scroll offset based on recipe count
        auto allRecipes = craftingSystem_.getAllRecipes();
        // NOTE: calculate by height of view
        int panelHeight = Constants::RECIPE_LIST_HEIGHT;
        int recipeItemHeight = Constants::RECIPE_ITEM_HEIGHT;
        int visibleRecipes = panelHeight / recipeItemHeight;
        int maxScrollOffset = std::max(0, static_cast<int>(allRecipes.size()) - visibleRecipes);
        
        // Clamp scroll offset
        craftingScrollOffset_ = std::max(0, std::min(craftingScrollOffset_, maxScrollOffset));
        
        std::cout << "Crafting panel scroll: " << craftingScrollOffset_ << "/" << maxScrollOffset << std::endl;
    } else if (view_.isPointInUIArea(x, y, "inventoryArea")) {
        // Scroll inventory card list only when mouse is over inventory area (pixel-based scrolling)
        inventoryScrollOffset_ -= deltaY * INVENTORY_SCROLL_SPEED;
        
        // Calculate maximum scroll offset based on card count and spacing within inventory bounds
        auto& cards = inventory_.getCards();
        // Calculate visible cards based on inventory area height instead of window height
        int inventoryHeight = Constants::INVENTORY_AREA_HEIGHT;
        int cardSpacing = Constants::CARD_SPACING;
        int visibleCards = inventoryHeight / cardSpacing;
        int totalCards = static_cast<int>(cards.size());
        
        // Calculate max scroll in pixels based on card spacing
        int maxScrollOffset = 0;
        if (totalCards > visibleCards) {
            maxScrollOffset = (totalCards - visibleCards) * Constants::CARD_SPACING;
        }
        
        // Clamp scroll offset
        inventoryScrollOffset_ = std::max(0, std::min(inventoryScrollOffset_, maxScrollOffset));
        
        std::cout << "Inventory scroll: " << inventoryScrollOffset_ << "/" << maxScrollOffset << std::endl;
    } else {
        // Mouse is not over any scrollable area
        std::cout << "Scroll event ignored - mouse not over scrollable area" << std::endl;
    }
}

void GameInputHandler::updateUICardSelection() {
    // Update UICard visual selection states to match game logic selection
    view_.setCardSelection(selectedCard_);
    
    // Update tracking for future reference
    previousSelectedCard_ = selectedCard_;
}

// Focus management request methods
void GameInputHandler::requestFocusNext() {
    if (focusNextCallback_) {
        focusNextCallback_();
    }
}

void GameInputHandler::requestFocusPrevious() {
    if (focusPreviousCallback_) {
        focusPreviousCallback_();
    }
}

void GameInputHandler::requestClearFocus() {
    if (clearFocusCallback_) {
        clearFocusCallback_();
    }
}

// Drag and drop implementation
void GameInputHandler::startDrag(Card* card, int startX, int startY) {
    if (!card || !baseBuildingController_) return;
    
    isDragging_ = true;
    draggedCard_ = card;
    dragStartX_ = startX;
    dragStartY_ = startY;
    
    std::cout << "Started dragging card: " << card->name << " from (" << startX << ", " << startY << ")" << std::endl;
}

void GameInputHandler::updateDrag(int currentX, int currentY) {
    if (!isDragging_ || !draggedCard_) return;
    
    // Visual feedback could be added here
    // For example, highlighting valid drop zones
    if (baseBuildingController_->isInBaseArea(currentX, currentY)) {
        auto [gridX, gridY] = baseBuildingController_->calculateGridPosition(currentX, currentY);
        // Could highlight grid cell at (gridX, gridY) as potential drop target
    }
}

void GameInputHandler::endDrag(int endX, int endY) {
    if (!isDragging_ || !draggedCard_ || !baseBuildingController_) {
        isDragging_ = false;
        draggedCard_ = nullptr;
        return;
    }
    
    std::cout << "Ending drag at (" << endX << ", " << endY << ")" << std::endl;
    
    // Try to place building if dropped in base area
    bool placementSuccess = baseBuildingController_->handleCardDrop(draggedCard_, endX, endY);
    
    if (placementSuccess) {
        std::cout << "Successfully placed building from dragged card!" << std::endl;
        // Clear selection after successful placement
        selectedCard_ = nullptr;
        updateUICardSelection();
    } else {
        std::cout << "Failed to place building: " << baseBuildingController_->getErrorMessage(baseBuildingController_->getLastError()) << std::endl;
    }
    
    // Reset drag state
    isDragging_ = false;
    draggedCard_ = nullptr;
}

bool GameInputHandler::shouldStartDrag(int currentX, int currentY) const {
    if (!selectedCard_) return false;
    
    int deltaX = currentX - dragStartX_;
    int deltaY = currentY - dragStartY_;
    int distance = deltaX * deltaX + deltaY * deltaY;
    
    return distance >= (DRAG_THRESHOLD * DRAG_THRESHOLD);
}

void GameInputHandler::validateCardPointers() {
    // Helper function to check if a card pointer is valid
    auto isCardValid = [this](const Card* card) -> bool {
        if (!card) return false;
        const auto& cards = inventory_.getCards();
        for (const auto& invCard : cards) {
            if (&invCard == card) {
                return true;
            }
        }
        return false;
    };
    
    // Clear selectedCard_ if it's no longer valid
    if (selectedCard_ && !isCardValid(selectedCard_)) {
        selectedCard_ = nullptr;
    }
    
    // Clear previousSelectedCard_ if it's no longer valid
    if (previousSelectedCard_ && !isCardValid(previousSelectedCard_)) {
        previousSelectedCard_ = nullptr;
    }
    
    // Clear draggedCard_ and stop dragging if it's no longer valid
    if (draggedCard_ && !isCardValid(draggedCard_)) {
        draggedCard_ = nullptr;
        isDragging_ = false;
    }
}
