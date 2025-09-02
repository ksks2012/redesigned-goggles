#include "GameInputHandler.h"
#include "Constants.h"
#include <iostream>
#include <random>

GameInputHandler::GameInputHandler(IGameView& view, 
                                  Inventory& inventory, 
                                  CraftingSystem& craftingSystem)
    : view_(view), inventory_(inventory), craftingSystem_(craftingSystem),
      running_(true), selectedCard_(nullptr), showCraftingPanel_(false),
      mouseX_(0), mouseY_(0), inventoryScrollOffset_(0), craftingScrollOffset_(0) {
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
            int recipeIndex = view_.getClickedRecipeIndex(x, y);
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
        const Card* hoveredCard = view_.getHoveredCard(inventory_, x, y);
        if (hoveredCard) {
            selectedCard_ = const_cast<Card*>(hoveredCard);
            handleCardClick(hoveredCard);
        } else {
            selectedCard_ = nullptr;
        }
    }
}

void GameInputHandler::handleMouseUp(int x, int y) {
    mouseX_ = x;
    mouseY_ = y;
    selectedCard_ = nullptr;
}

void GameInputHandler::handleMouseMotion(int x, int y) {
    mouseX_ = x;
    mouseY_ = y;
}

void GameInputHandler::handleMouseWheel(int x, int y, int deltaY) {
    mouseX_ = x;
    mouseY_ = y;
    handleScrollWheel(x, y, deltaY);
}

void GameInputHandler::handleKeyDown(int keyCode) {
    switch (keyCode) {
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
            // ESC to close crafting panel or exit game
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
    std::cout << "Selected card: " << card->name << std::endl;
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
        inventory_.removeCard(cards[0].name, cards[0].rarity);
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
    const int SCROLL_SPEED = 3; // Cards to scroll per wheel notch
    
    // Determine which area is being scrolled based on mouse position
    if (showCraftingPanel_ && view_.isCraftingPanelHovered(x, y)) {
        // Scroll crafting panel recipes
        craftingScrollOffset_ -= deltaY * SCROLL_SPEED;
        
        // Calculate maximum scroll offset based on recipe count
        auto allRecipes = craftingSystem_.getAllRecipes();
        int visibleRecipes = 6; // Approximate visible recipes in panel
        int maxScrollOffset = std::max(0, static_cast<int>(allRecipes.size()) - visibleRecipes);
        
        // Clamp scroll offset
        craftingScrollOffset_ = std::max(0, std::min(craftingScrollOffset_, maxScrollOffset));
        
        std::cout << "Crafting panel scroll: " << craftingScrollOffset_ << "/" << maxScrollOffset << std::endl;
    } else {
        // Scroll inventory card list
        inventoryScrollOffset_ -= deltaY * SCROLL_SPEED;
        
        // Calculate maximum scroll offset based on card count
        auto& cards = inventory_.getCards();
        int visibleCards = 10; // Approximate visible cards in inventory
        int maxScrollOffset = std::max(0, static_cast<int>(cards.size()) - visibleCards);
        
        // Clamp scroll offset
        inventoryScrollOffset_ = std::max(0, std::min(inventoryScrollOffset_, maxScrollOffset));
        
        std::cout << "Inventory scroll: " << inventoryScrollOffset_ << "/" << maxScrollOffset << std::endl;
    }
}
