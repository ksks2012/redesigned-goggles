#include "Interface/ui/UICraftingPanel.h"
#include "Systems/SDLManager.h"

// UIRecipeItem implementation
UIRecipeItem::UIRecipeItem(const Recipe& recipe, int x, int y, SDLManager& sdlManager, 
                           std::function<void(const Recipe&)> onRecipeClick)
    : UIComponent(x, y, Constants::CRAFT_PANEL_WIDTH - Constants::RECIPE_ITEM_MARGIN,
                  Constants::RECIPE_ITEM_HEIGHT - Constants::RECIPE_ITEM_VERTICAL_SPACING, sdlManager),
      recipe_(recipe),
      canCraft_(false),
      onRecipeClick_(onRecipeClick) {
}

void UIRecipeItem::render() {
    // Background color based on craft availability
    SDL_Color bgColor = canCraft_ ? Constants::RECIPE_CAN_CRAFT_BG : Constants::RECIPE_CANNOT_CRAFT_BG;
    if (!recipe_.isUnlocked) {
        bgColor = Constants::RECIPE_LOCKED_BG;
    }
    
    renderBackground(bgColor);
    renderBorder(Constants::BORDER_COLOR);
    
    // Text color based on availability
    SDL_Color textColor = canCraft_ ? Constants::TEXT_COLOR : Constants::RECIPE_DISABLED_TEXT;
    if (!recipe_.isUnlocked) {
        textColor = Constants::BORDER_COLOR;
        renderText("??? (Locked)", Constants::CRAFT_PANEL_MARGIN, 5, textColor);
    } else {
        renderText(recipe_.name, Constants::CRAFT_PANEL_MARGIN, 5, textColor);
        
        // Success rate
        std::string successText = "Success Rate: " + std::to_string(static_cast<int>(recipe_.successRate * 100)) + "%";
        renderText(successText, Constants::CRAFT_PANEL_MARGIN, Constants::CRAFT_SUCCESS_RATE_OFFSET_Y, 
                   Constants::ATTRIBUTE_TEXT_COLOR);
        
        // Ingredients
        renderIngredientsList(Constants::CRAFT_INGREDIENT_OFFSET_X, 5);
        
        // Result
        std::string resultText = "→ " + recipe_.result.name;
        renderText(resultText, Constants::CRAFT_INGREDIENT_OFFSET_X, Constants::CRAFT_RESULT_OFFSET_Y, textColor);
    }
}

void UIRecipeItem::update(const Recipe& recipe, bool canCraft) {
    recipe_ = recipe;
    canCraft_ = canCraft;
}

void UIRecipeItem::handleClick(int mouseX, int mouseY) {
    if (isPointInside(mouseX, mouseY) && canCraft_ && recipe_.isUnlocked && onRecipeClick_) {
        onRecipeClick_(recipe_);
    }
}

void UIRecipeItem::renderIngredientsList(int x, int y) {
    std::string ingredientsText = "Requires: ";
    
    for (size_t i = 0; i < recipe_.ingredients.size(); ++i) {
        const auto& ingredient = recipe_.ingredients[i];
        ingredientsText += ingredient.first.name + " x" + std::to_string(ingredient.second);
        
        if (i < recipe_.ingredients.size() - 1) {
            ingredientsText += ", ";
        }
    }
    
    renderText(ingredientsText, x, y, Constants::SECONDARY_TEXT_COLOR);
}

// UICraftingPanel implementation
UICraftingPanel::UICraftingPanel(SDLManager& sdlManager, 
                                 std::function<void(const Recipe&)> onRecipeClick)
    : UIComponent(Constants::CRAFT_PANEL_X, Constants::CRAFT_PANEL_Y,
                  Constants::CRAFT_PANEL_WIDTH, Constants::CRAFT_PANEL_HEIGHT, sdlManager),
      visible_(false),
      scrollOffset_(0),
      onRecipeClick_(onRecipeClick) {
}

void UICraftingPanel::render() {
    if (!visible_) {
        return;
    }
    
    renderOverlay();
    renderPanelBackground();
    renderTitle();
    renderCloseHint();
    
    // Render visible recipe items with scroll offset
    int startY = Constants::CRAFT_PANEL_Y + Constants::CRAFT_PANEL_RECIPES_START_Y;
    int itemHeight = Constants::RECIPE_ITEM_HEIGHT;
    int visibleItems = (Constants::CRAFT_PANEL_HEIGHT - Constants::CRAFT_PANEL_RECIPES_START_Y - 20) / itemHeight;
    int panelBottom = Constants::CRAFT_PANEL_Y + Constants::CRAFT_PANEL_HEIGHT - 20;
    
    for (int i = scrollOffset_; i < static_cast<int>(recipeItems_.size()) && i < scrollOffset_ + visibleItems; ++i) {
        if (recipeItems_[i]) {
            // Adjust recipe item position based on scroll offset
            int displayIndex = i - scrollOffset_;
            int itemY = startY + displayIndex * itemHeight;
            
            // Only render items that are within the panel bounds
            if (itemY >= startY && itemY + itemHeight <= panelBottom) {
                recipeItems_[i]->setPosition(Constants::CRAFT_PANEL_X + Constants::CRAFT_PANEL_MARGIN, itemY);
                recipeItems_[i]->render();
            }
        }
    }
    
    // Render scroll indicator if needed
    if (recipeItems_.size() > visibleItems) {
        renderScrollIndicator();
    }
}

void UICraftingPanel::update(const CraftingSystem& craftingSystem, const Inventory& inventory, int scrollOffset) {
    if (!visible_) {
        return;
    }
    
    scrollOffset_ = scrollOffset;
    auto allRecipes = craftingSystem.getAllRecipes();
    
    // Create recipe items if needed
    if (recipeItems_.size() != allRecipes.size()) {
        createRecipeItems(allRecipes);
    }
    
    // Update recipe items with current status
    for (size_t i = 0; i < recipeItems_.size() && i < allRecipes.size(); ++i) {
        const Recipe& recipe = allRecipes[i];
        bool canCraft = false;
        
        if (recipe.isUnlocked) {
            canCraft = true;
            for (const auto& ingredient : recipe.ingredients) {
                const Card& requiredCard = ingredient.first;
                int requiredQuantity = ingredient.second;
                
                bool hasEnough = false;
                for (const auto& card : inventory.getCards()) {
                    if (card.name == requiredCard.name && 
                        card.rarity == requiredCard.rarity &&
                        card.quantity >= requiredQuantity) {
                        hasEnough = true;
                        break;
                    }
                }
                
                if (!hasEnough) {
                    canCraft = false;
                    break;
                }
            }
        }
        
        recipeItems_[i]->update(recipe, canCraft);
    }
}

void UICraftingPanel::handleClick(int mouseX, int mouseY) {
    if (!visible_) {
        return;
    }
    
    // Handle recipe item clicks
    for (auto& recipeItem : recipeItems_) {
        recipeItem->handleClick(mouseX, mouseY);
    }
}

void UICraftingPanel::show() {
    visible_ = true;
}

void UICraftingPanel::hide() {
    visible_ = false;
}

void UICraftingPanel::createRecipeItems(const std::vector<Recipe>& recipes) {
    recipeItems_.clear();
    
    int startY = Constants::CRAFT_PANEL_Y + Constants::CRAFT_PANEL_RECIPES_START_Y;
    
    // Create all recipe items regardless of panel height - scrolling will handle visibility
    for (size_t i = 0; i < recipes.size(); ++i) {
        const Recipe& recipe = recipes[i];
        
        // Calculate initial position (will be adjusted during scrolling)
        int itemY = startY + i * Constants::RECIPE_ITEM_HEIGHT;
        
        auto recipeItem = std::make_unique<UIRecipeItem>(
            recipe, 
            Constants::CRAFT_PANEL_X + Constants::CRAFT_PANEL_MARGIN, 
            itemY, 
            sdlManager_, 
            onRecipeClick_
        );
        
        recipeItems_.push_back(std::move(recipeItem));
    }
}

void UICraftingPanel::renderOverlay() {
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_Rect overlay = {0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT};
    SDL_SetRenderDrawColor(renderer, Constants::OVERLAY_COLOR.r, Constants::OVERLAY_COLOR.g, 
                           Constants::OVERLAY_COLOR.b, Constants::OVERLAY_COLOR.a);
    SDL_RenderFillRect(renderer, &overlay);
}

void UICraftingPanel::renderPanelBackground() {
    renderBackground(Constants::PANEL_BG_COLOR);
    renderBorder(Constants::BORDER_COLOR);
}

void UICraftingPanel::renderTitle() {
    renderText("Crafting System", Constants::CRAFT_PANEL_TITLE_OFFSET_X, 
               Constants::CRAFT_PANEL_TITLE_OFFSET_Y, Constants::TEXT_COLOR);
}

void UICraftingPanel::renderCloseHint() {
    renderText("Press C, ESC or click outside to close", 
               Constants::CRAFT_PANEL_TITLE_OFFSET_X, 
               Constants::CRAFT_PANEL_HINT_OFFSET_Y, 
               Constants::SECONDARY_TEXT_COLOR);
}

void UICraftingPanel::renderScrollIndicator() {
    if (recipeItems_.empty()) return;
    
    // Calculate scroll indicator dimensions
    int scrollBarX = Constants::CRAFT_PANEL_X + Constants::CRAFT_PANEL_WIDTH - 15;
    int scrollBarY = Constants::CRAFT_PANEL_Y + Constants::CRAFT_PANEL_RECIPES_START_Y;
    int scrollBarHeight = Constants::CRAFT_PANEL_HEIGHT - Constants::CRAFT_PANEL_RECIPES_START_Y - 20;
    
    // Background track
    SDL_Rect trackRect = {scrollBarX, scrollBarY, 10, scrollBarHeight};
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 100, 100, 100, 128);
    SDL_RenderFillRect(sdlManager_.getRenderer(), &trackRect);
    
    // Calculate thumb position and size
    int visibleItems = scrollBarHeight / Constants::RECIPE_ITEM_HEIGHT;
    int totalItems = static_cast<int>(recipeItems_.size());
    
    if (totalItems > visibleItems) {
        float thumbHeight = (static_cast<float>(visibleItems) / totalItems) * scrollBarHeight;
        float thumbPosition = (static_cast<float>(scrollOffset_) / (totalItems - visibleItems)) * (scrollBarHeight - thumbHeight);
        
        SDL_Rect thumbRect = {scrollBarX + 1, 
                             static_cast<int>(scrollBarY + thumbPosition), 
                             8, 
                             static_cast<int>(thumbHeight)};
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 200, 200, 200, 255);
        SDL_RenderFillRect(sdlManager_.getRenderer(), &thumbRect);
    }
}
