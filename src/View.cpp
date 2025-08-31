#include "View.h"
#include "Constants.h"
#include "utils.h"
#include <iostream>

View::View(SDLManager& sdl) : sdlManager(sdl) {}

void View::render(const Inventory& inventory, const Card* selectedCard, int mouseX, int mouseY, 
                  bool showCraftingPanel, const CraftingSystem& craftingSystem) {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    SDL_SetRenderDrawColor(renderer, Constants::BACKGROUND_COLOR.r, Constants::BACKGROUND_COLOR.g,
                           Constants::BACKGROUND_COLOR.b, Constants::BACKGROUND_COLOR.a);
    SDL_RenderClear(renderer);

    int index = 0;
    for (const auto& card : inventory.getCards()) {
        int cardY = Constants::CARD_X + index * Constants::CARD_SPACING;
        SDL_Rect rect = {Constants::CARD_X, cardY, Constants::CARD_WIDTH, Constants::CARD_HEIGHT};
        setRarityColor(card.rarity);
        SDL_RenderFillRect(renderer, &rect);
        
        // Only display card name, quantity, and type (remove attribute display)
        std::string mainText = card.name + " x" + std::to_string(card.quantity) + " (" + card.getTypeString() + ")";
        renderText(mainText, Constants::CARD_X + Constants::CARD_TEXT_OFFSET_X, cardY + Constants::CARD_TEXT_OFFSET_Y,
                   Constants::TEXT_COLOR);
        
        index++;
    }        if (selectedCard) {
        SDL_Rect dragRect = {mouseX + Constants::DRAG_CARD_OFFSET_X, mouseY + Constants::DRAG_CARD_OFFSET_Y,
                             Constants::CARD_WIDTH, Constants::CARD_HEIGHT};
        setRarityColor(selectedCard->rarity);
        SDL_RenderFillRect(renderer, &dragRect);
        
        // Only show basic info when dragging
        std::string mainText = selectedCard->name + " x" + std::to_string(selectedCard->quantity) + " (" + selectedCard->getTypeString() + ")";
        renderText(mainText, mouseX + Constants::DRAG_TEXT_OFFSET_X, mouseY + Constants::DRAG_TEXT_OFFSET_Y,
                   Constants::TEXT_COLOR);
    }

    renderButton(Constants::BUTTON_TEXT_ADD, Constants::BUTTON_X, Constants::BUTTON_Y_ADD,
                 Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT);
    renderButton(Constants::BUTTON_TEXT_REMOVE, Constants::BUTTON_X, Constants::BUTTON_Y_REMOVE,
                 Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT);
    renderButton(Constants::BUTTON_TEXT_EXPLORE, Constants::BUTTON_X, Constants::BUTTON_Y_EXPLORE,
                 Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT);
    renderButton(Constants::BUTTON_TEXT_CRAFT, Constants::BUTTON_X, Constants::BUTTON_Y_CRAFT,
                 Constants::BUTTON_MIN_WIDTH, Constants::BUTTON_HEIGHT);

    // Render crafting panel
    if (showCraftingPanel) {
        renderCraftingPanel(craftingSystem, inventory);
    }

    // Show operation hints
    int hintX = Constants::HINT_X;
    int hintY = Constants::WINDOW_HEIGHT - Constants::HINT_BOTTOM_OFFSET;
    renderText(Constants::HINT_TITLE, hintX, hintY, Constants::TEXT_COLOR);
    renderText(Constants::HINT_SAVE, hintX, hintY + Constants::HINT_LINE_SPACING, Constants::TEXT_COLOR);
    renderText(Constants::HINT_LOAD, hintX, hintY + 2 * Constants::HINT_LINE_SPACING, Constants::TEXT_COLOR);
    renderText(Constants::HINT_EXIT, hintX, hintY + 3 * Constants::HINT_LINE_SPACING, Constants::TEXT_COLOR);

    // Detect mouse hover and show tooltip (only when not dragging a card and crafting panel is not shown)
    if (!selectedCard && !showCraftingPanel) {
        const Card* hoveredCard = getHoveredCard(inventory, mouseX, mouseY);
        if (hoveredCard) {
            renderTooltip(*hoveredCard, mouseX, mouseY);
        }
    }

    SDL_RenderPresent(renderer);
}

void View::setRarityColor(int rarity) {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    SDL_Color color;
    switch (rarity) {
        case 1: color = Constants::RARITY_COMMON; break;
        case 2: color = Constants::RARITY_RARE; break;
        case 3: color = Constants::RARITY_LEGENDARY; break;
        default: color = Constants::RARITY_COMMON;
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void View::renderText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderUTF8_Solid(sdlManager.getFont(), text.c_str(), color);
    if (!surface) {
        std::cerr << "Text rendering failed: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlManager.getRenderer(), surface);
    if (!texture) {
        std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    int textW, textH;
    TTF_SizeUTF8(sdlManager.getFont(), text.c_str(), &textW, &textH);
    SDL_Rect dst = {x, y, textW, textH};
    SDL_RenderCopy(sdlManager.getRenderer(), texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void View::renderButton(const std::string& text, int x, int y, int minW, int h) {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    int textW, textH;
    TTF_SizeUTF8(sdlManager.getFont(), text.c_str(), &textW, &textH);
    int buttonW = std::max(minW, textW + Constants::BUTTON_TEXT_PADDING);
    SDL_Rect rect = {x, y, buttonW, h};
    SDL_SetRenderDrawColor(renderer, Constants::BUTTON_COLOR.r, Constants::BUTTON_COLOR.g,
                           Constants::BUTTON_COLOR.b, Constants::BUTTON_COLOR.a);
    SDL_RenderFillRect(renderer, &rect);
    int textX = x + (buttonW - textW) / 2;
    int textY = y + (h - textH) / 2;
    renderText(text, textX, textY, Constants::TEXT_COLOR);
}

std::string View::getCardAttributeText(const Card& card) {
    std::string attributeText = "";

    // Display the most important attribute based on card type
    switch (card.type) {
        case CardType::FOOD:
            if (card.hasAttribute(AttributeType::NUTRITION)) {
                attributeText += "Nutrition: " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::NUTRITION)));
            }
            break;

        case CardType::WEAPON:
            if (card.hasAttribute(AttributeType::ATTACK)) {
                attributeText += "Attack: " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::ATTACK)));
            }
            if (card.hasAttribute(AttributeType::DURABILITY)) {
                if (!attributeText.empty()) attributeText += " ";
                attributeText += "Durability: " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::DURABILITY)));
            }
            break;

        case CardType::ARMOR:
            if (card.hasAttribute(AttributeType::DEFENSE)) {
                attributeText += "Defense: " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::DEFENSE)));
            }
            if (card.hasAttribute(AttributeType::DURABILITY)) {
                if (!attributeText.empty()) attributeText += " ";
                attributeText += "Durability: " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::DURABILITY)));
            }
            break;

        case CardType::HERB:
            if (card.hasAttribute(AttributeType::HEALING)) {
                attributeText += "Healing: " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::HEALING)));
            }
            break;

        case CardType::FUEL:
            if (card.hasAttribute(AttributeType::BURN_VALUE)) {
                attributeText += "Burn Value: " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::BURN_VALUE)));
            }
            break;

        case CardType::METAL:
        case CardType::BUILDING:
            if (card.hasAttribute(AttributeType::CRAFTING_VALUE)) {
                attributeText += "Crafting Value: " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::CRAFTING_VALUE)));
            }
            break;

        default:
            // For other types, display weight or trade value
            if (card.hasAttribute(AttributeType::WEIGHT)) {
                attributeText += "Weight: " + std::to_string(card.getAttribute(AttributeType::WEIGHT));
            }
            break;
    }

    // Always display total weight (if weight attribute exists)
    if (card.hasAttribute(AttributeType::WEIGHT) && card.quantity > 1) {
        if (!attributeText.empty()) attributeText += " ";
        attributeText += "Total Weight: " + std::to_string(card.getTotalWeight());
    }

    return attributeText;
}

const Card* View::getHoveredCard(const Inventory& inventory, int mouseX, int mouseY) {
    int index = 0;
    for (const auto& card : inventory.getCards()) {
        int cardY = Constants::CARD_X + index * Constants::CARD_SPACING;
        
        // Check if mouse is within card area
        if (mouseX >= Constants::CARD_X && mouseX <= Constants::CARD_X + Constants::CARD_WIDTH &&
            mouseY >= cardY && mouseY <= cardY + Constants::CARD_HEIGHT) {
            return &card;
        }
        index++;
    }
    return nullptr;
}

void View::renderTooltip(const Card& card, int mouseX, int mouseY) {
    std::vector<std::string> tooltipLines = getDetailedCardInfo(card);
    if (tooltipLines.empty()) return;

    // Calculate tooltip window size
    int maxWidth = 0;
    
    for (const auto& line : tooltipLines) {
        int textW, textH;
        TTF_SizeUTF8(sdlManager.getFont(), line.c_str(), &textW, &textH);
        maxWidth = std::max(maxWidth, textW);
    }
    
    int tooltipWidth = maxWidth + 2 * Constants::TOOLTIP_PADDING;
    int tooltipHeight = tooltipLines.size() * Constants::TOOLTIP_LINE_HEIGHT + 2 * Constants::TOOLTIP_PADDING;
    
    // Adjust tooltip position to avoid going off screen edges
    int tooltipX = mouseX + Constants::TOOLTIP_MOUSE_OFFSET; // Slight offset to avoid being covered by the mouse
    int tooltipY = mouseY - tooltipHeight / 2;
    
    if (tooltipX + tooltipWidth > Constants::WINDOW_WIDTH) {
        tooltipX = mouseX - tooltipWidth - Constants::TOOLTIP_MOUSE_OFFSET;
    }
    if (tooltipY < 0) {
        tooltipY = Constants::TOOLTIP_SCREEN_MARGIN;
    }
    if (tooltipY + tooltipHeight > Constants::WINDOW_HEIGHT) {
        tooltipY = Constants::WINDOW_HEIGHT - tooltipHeight - Constants::TOOLTIP_SCREEN_MARGIN;
    }
    
    // Render tooltip background
    renderTooltipBackground(tooltipX, tooltipY, tooltipWidth, tooltipHeight);

    // Render text
    for (size_t i = 0; i < tooltipLines.size(); ++i) {
        SDL_Color textColor = (i == 0) ? Constants::TEXT_COLOR : Constants::ATTRIBUTE_TEXT_COLOR;
        renderText(tooltipLines[i], tooltipX + Constants::TOOLTIP_PADDING, 
                   tooltipY + Constants::TOOLTIP_PADDING + i * Constants::TOOLTIP_LINE_HEIGHT, textColor);
    }
}

void View::renderTooltipBackground(int x, int y, int width, int height) {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    
    // Background
    SDL_Rect bgRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, Constants::TOOLTIP_BG_COLOR.r, Constants::TOOLTIP_BG_COLOR.g, Constants::TOOLTIP_BG_COLOR.b, Constants::TOOLTIP_BG_COLOR.a);
    SDL_RenderFillRect(renderer, &bgRect);
    
    // Border
    SDL_SetRenderDrawColor(renderer, Constants::ATTRIBUTE_TEXT_COLOR.r, Constants::ATTRIBUTE_TEXT_COLOR.g, Constants::ATTRIBUTE_TEXT_COLOR.b, Constants::ATTRIBUTE_TEXT_COLOR.a);
    SDL_RenderDrawRect(renderer, &bgRect);
}

std::vector<std::string> View::getDetailedCardInfo(const Card& card) {
    std::vector<std::string> info;

    // Title: name and type
    info.push_back(card.name + " (" + card.getTypeString() + ")");

    // Basic info
    info.push_back(Constants::TOOLTIP_RARITY + ": " + std::to_string(card.rarity) +
                   (card.rarity == 1 ? " (" + Constants::TOOLTIP_COMMON + ")" :
                    card.rarity == 2 ? " (" + Constants::TOOLTIP_RARE + ")" : " (" + Constants::TOOLTIP_LEGENDARY + ")"));

    info.push_back(Constants::TOOLTIP_QUANTITY + ": " + std::to_string(card.quantity));

    // Attribute info
    if (card.hasAttribute(AttributeType::WEIGHT)) {
        info.push_back(Constants::TOOLTIP_WEIGHT + ": " + to_string_with_precision(card.getAttribute(AttributeType::WEIGHT), 2) + "kg");
        if (card.quantity > 1) {
            info.push_back(Constants::TOOLTIP_TOTAL_WEIGHT + ": " + to_string_with_precision(card.getTotalWeight(), 2) + "kg");
        }
    }

    if (card.hasAttribute(AttributeType::NUTRITION)) {
        info.push_back(Constants::TOOLTIP_NUTRITION + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::NUTRITION))));
    }

    if (card.hasAttribute(AttributeType::ATTACK)) {
        info.push_back(Constants::TOOLTIP_ATTACK + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::ATTACK))));
    }

    if (card.hasAttribute(AttributeType::DEFENSE)) {
        info.push_back(Constants::TOOLTIP_DEFENSE + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::DEFENSE))));
    }

    if (card.hasAttribute(AttributeType::HEALING)) {
        info.push_back(Constants::TOOLTIP_HEALING + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::HEALING))));
    }

    if (card.hasAttribute(AttributeType::DURABILITY)) {
        info.push_back(Constants::TOOLTIP_DURABILITY + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::DURABILITY))));
    }

    if (card.hasAttribute(AttributeType::BURN_VALUE)) {
        info.push_back(Constants::TOOLTIP_BURN_VALUE + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::BURN_VALUE))));
    }

    if (card.hasAttribute(AttributeType::CRAFTING_VALUE)) {
        info.push_back(Constants::TOOLTIP_CRAFTING_VALUE + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::CRAFTING_VALUE))));
    }

    if (card.hasAttribute(AttributeType::TRADE_VALUE)) {
        info.push_back(Constants::TOOLTIP_TRADE_VALUE + ": " + std::to_string(static_cast<int>(card.getAttribute(AttributeType::TRADE_VALUE))));
    }

    // Functional info
    if (card.isEdible()) {
        info.push_back(Constants::TOOLTIP_EDIBLE);
    }

    if (card.isBurnable()) {
        info.push_back(Constants::TOOLTIP_BURNABLE);
    }

    return info;
}

void View::renderCraftingPanel(const CraftingSystem& craftingSystem, const Inventory& inventory) {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    
    // Render semi-transparent background overlay
    SDL_Rect overlay = {0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT};
    SDL_SetRenderDrawColor(renderer, Constants::OVERLAY_COLOR.r, Constants::OVERLAY_COLOR.g, 
                           Constants::OVERLAY_COLOR.b, Constants::OVERLAY_COLOR.a);
    SDL_RenderFillRect(renderer, &overlay);
    
    // Render crafting panel background
    SDL_Rect panelRect = {Constants::CRAFT_PANEL_X, Constants::CRAFT_PANEL_Y, 
                          Constants::CRAFT_PANEL_WIDTH, Constants::CRAFT_PANEL_HEIGHT};
    SDL_SetRenderDrawColor(renderer, Constants::PANEL_BG_COLOR.r, Constants::PANEL_BG_COLOR.g, 
                           Constants::PANEL_BG_COLOR.b, Constants::PANEL_BG_COLOR.a);
    SDL_RenderFillRect(renderer, &panelRect);
    
    // Render border
    SDL_SetRenderDrawColor(renderer, Constants::BORDER_COLOR.r, Constants::BORDER_COLOR.g, 
                           Constants::BORDER_COLOR.b, Constants::BORDER_COLOR.a);
    SDL_RenderDrawRect(renderer, &panelRect);
    
    // Title
    renderText("Crafting System", Constants::CRAFT_PANEL_X + Constants::CRAFT_PANEL_TITLE_OFFSET_X, 
               Constants::CRAFT_PANEL_Y + Constants::CRAFT_PANEL_TITLE_OFFSET_Y, Constants::TEXT_COLOR);
    
    // Close hint
    renderText("Press C or click blank area to close", 
               Constants::CRAFT_PANEL_X + Constants::CRAFT_PANEL_TITLE_OFFSET_X, 
               Constants::CRAFT_PANEL_Y + Constants::CRAFT_PANEL_HINT_OFFSET_Y, 
               Constants::SECONDARY_TEXT_COLOR);
    
    // Get all recipes and render
    auto allRecipes = craftingSystem.getAllRecipes();
    renderRecipeList(allRecipes, inventory);
}

void View::renderRecipeList(const std::vector<Recipe>& recipes, const Inventory& inventory) {
    int startY = Constants::CRAFT_PANEL_Y + Constants::CRAFT_PANEL_RECIPES_START_Y;
    int currentY = startY;
    
    for (size_t i = 0; i < recipes.size(); ++i) {
        const Recipe& recipe = recipes[i];
        bool canCraft = false;
        
        // Check if crafting is possible
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
        
        renderRecipeItem(recipe, Constants::CRAFT_PANEL_X + Constants::CRAFT_PANEL_MARGIN, currentY, canCraft);
        currentY += Constants::RECIPE_ITEM_HEIGHT;
        
        // Prevent exceeding panel range
        if (currentY + Constants::RECIPE_ITEM_HEIGHT > Constants::CRAFT_PANEL_Y + Constants::CRAFT_PANEL_HEIGHT - Constants::CRAFT_PANEL_BOTTOM_MARGIN) {
            break;
        }
    }
}

void View::renderRecipeItem(const Recipe& recipe, int x, int y, bool canCraft) {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    
    // Background color changes depending on whether crafting is possible
    SDL_Color bgColor = canCraft ? Constants::RECIPE_CAN_CRAFT_BG : Constants::RECIPE_CANNOT_CRAFT_BG;
    if (!recipe.isUnlocked) {
        bgColor = Constants::RECIPE_LOCKED_BG;
    }
    
    SDL_Rect itemRect = {x, y, Constants::CRAFT_PANEL_WIDTH - Constants::RECIPE_ITEM_MARGIN, 
                         Constants::RECIPE_ITEM_HEIGHT - Constants::RECIPE_ITEM_VERTICAL_SPACING};
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &itemRect);
    
    // Border
    SDL_SetRenderDrawColor(renderer, Constants::BORDER_COLOR.r, Constants::BORDER_COLOR.g, 
                           Constants::BORDER_COLOR.b, Constants::BORDER_COLOR.a);
    SDL_RenderDrawRect(renderer, &itemRect);
    
    // Recipe name
    SDL_Color textColor = canCraft ? Constants::TEXT_COLOR : Constants::RECIPE_DISABLED_TEXT;
    if (!recipe.isUnlocked) {
        textColor = Constants::BORDER_COLOR;
        renderText("??? (Locked)", x + Constants::CRAFT_PANEL_MARGIN, y + 5, textColor);
    } else {
        renderText(recipe.name, x + Constants::CRAFT_PANEL_MARGIN, y + 5, textColor);
        
        // Success rate
        std::string successText = "Success Rate: " + std::to_string(static_cast<int>(recipe.successRate * 100)) + "%";
        renderText(successText, x + Constants::CRAFT_PANEL_MARGIN, y + Constants::CRAFT_SUCCESS_RATE_OFFSET_Y, 
                   Constants::ATTRIBUTE_TEXT_COLOR);
        
        // Ingredient requirements
        renderIngredientsList(recipe, x + Constants::CRAFT_INGREDIENT_OFFSET_X, y + 5);
        
        // Result
        std::string resultText = "→ " + recipe.result.name;
        renderText(resultText, x + Constants::CRAFT_INGREDIENT_OFFSET_X, y + Constants::CRAFT_RESULT_OFFSET_Y, textColor);
    }
}

void View::renderIngredientsList(const Recipe& recipe, int x, int y) {
    std::string ingredientsText = "Requires: ";
    
    for (size_t i = 0; i < recipe.ingredients.size(); ++i) {
        const auto& ingredient = recipe.ingredients[i];
        ingredientsText += ingredient.first.name + " x" + std::to_string(ingredient.second);
        
        if (i < recipe.ingredients.size() - 1) {
            ingredientsText += ", ";
        }
    }
    
    renderText(ingredientsText, x, y, Constants::SECONDARY_TEXT_COLOR);
}