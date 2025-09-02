#pragma once
#include "UIComponent.h"
#include "CraftingSystem.h"
#include "Inventory.h"
#include <vector>
#include <functional>

/**
 * Recipe item component for displaying individual recipes
 */
class UIRecipeItem : public UIComponent {
public:
    UIRecipeItem(const Recipe& recipe, int x, int y, SDLManager& sdlManager, 
                 std::function<void(const Recipe&)> onRecipeClick = nullptr);
    
    void render() override;
    void update(const Recipe& recipe, bool canCraft);
    void handleClick(int mouseX, int mouseY);

private:
    Recipe recipe_;
    bool canCraft_;
    std::function<void(const Recipe&)> onRecipeClick_;
    
    void renderIngredientsList(int x, int y);
};

/**
 * Crafting panel UI component
 */
class UICraftingPanel : public UIComponent {
public:
    UICraftingPanel(SDLManager& sdlManager, 
                    std::function<void(const Recipe&)> onRecipeClick = nullptr);
    
    void render() override;
    void update(const CraftingSystem& craftingSystem, const Inventory& inventory, int scrollOffset = 0);
    void handleClick(int mouseX, int mouseY);
    
    // Panel management
    void show();
    void hide();
    bool isVisible() const { return visible_; }
    
    // Scroll support
    void setScrollOffset(int scrollOffset) { scrollOffset_ = scrollOffset; }
    int getScrollOffset() const { return scrollOffset_; }

private:
    bool visible_;
    int scrollOffset_;  // Current scroll offset for recipes
    std::vector<std::unique_ptr<UIRecipeItem>> recipeItems_;
    std::function<void(const Recipe&)> onRecipeClick_;
    
    void createRecipeItems(const std::vector<Recipe>& recipes);
    void renderOverlay();
    void renderPanelBackground();
    void renderTitle();
    void renderCloseHint();
    void renderScrollIndicator();
};