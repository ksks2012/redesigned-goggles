#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "SDLManager.h"
#include "Inventory.h"
#include "Card.h"
#include "CraftingSystem.h"

class View {
public:
    View(SDLManager& sdl);
    void render(const Inventory& inventory, const Card* selectedCard, int mouseX, int mouseY, 
                bool showCraftingPanel, const CraftingSystem& craftingSystem);

private:
    SDLManager& sdlManager;

    void setRarityColor(int rarity);
    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void renderButton(const std::string& text, int x, int y, int minW, int h);
    std::string getCardAttributeText(const Card& card);
    
    // Tooltip related methods
    const Card* getHoveredCard(const Inventory& inventory, int mouseX, int mouseY);
    void renderTooltip(const Card& card, int mouseX, int mouseY);
    void renderTooltipBackground(int x, int y, int width, int height);
    std::vector<std::string> getDetailedCardInfo(const Card& card);
    
    // Crafting system related methods
    void renderCraftingPanel(const CraftingSystem& craftingSystem, const Inventory& inventory);
    void renderRecipeList(const std::vector<Recipe>& recipes, const Inventory& inventory);
    void renderRecipeItem(const Recipe& recipe, int x, int y, bool canCraft);
    void renderIngredientsList(const Recipe& recipe, int x, int y);
};