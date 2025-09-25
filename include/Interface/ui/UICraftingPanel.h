#pragma once
#include "../../../extracted_libs/ui_framework/include/Interface/ui/ContainerCompat.h"
#include "Systems/CraftingSystem.h"
#include "Core/Inventory.h"
#include <vector>
#include <functional>

/**
 * Recipe item component for displaying individual recipes
 * Uses SimpleContainer for composition-based container functionality
 */
class UIRecipeItem : public SimpleContainer {
public:
    UIRecipeItem(const Recipe& recipe, int x, int y, SDLManager& sdlManager, 
                 std::function<void(const Recipe&)> onRecipeClick = nullptr);
    
    void render() override;
    void update(const Recipe& recipe, bool canCraft);
    void handleClick(int mouseX, int mouseY);
    
    // Layout management for recipe content
    void updateLayout();

private:
    Recipe recipe_;
    bool canCraft_;
    std::function<void(const Recipe&)> onRecipeClick_;
    
    void renderIngredientsList(int x, int y);
    void createRecipeContent();  // Create child components for recipe content
};

/**
 * Crafting panel UI component
 * Uses SimpleContainer for composition-based container functionality with auto-layout
 */
class UICraftingPanel : public SimpleContainer {
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
    
    // Scroll support (SimpleContainer provides built-in scrolling capabilities)
    void setScrollOffset(int scrollOffset) { 
        setScrollable(true);
        SimpleContainer::setScrollOffset(scrollOffset); 
    }
    int getScrollOffset() const { return SimpleContainer::getScrollOffset(); }
    
    // Layout management for panel content
    void updatePanelLayout();

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
    void createPanelContent();  // Create child components for panel content
};