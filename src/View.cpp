#include "View.h"
#include "Constants.h"
#include "utils.h"
#include <iostream>

View::View(SDLManager& sdl) : sdlManager(sdl) {}

void View::render(const Inventory& inventory, const Card* selectedCard, int mouseX, int mouseY) {
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

    // Display operation hints
    int hintX = Constants::HINT_X;
    int hintY = Constants::WINDOW_HEIGHT - Constants::HINT_BOTTOM_OFFSET;
    renderText(Constants::HINT_TITLE, hintX, hintY, Constants::TEXT_COLOR);
    renderText(Constants::HINT_SAVE, hintX, hintY + Constants::HINT_LINE_SPACING, Constants::TEXT_COLOR);
    renderText(Constants::HINT_LOAD, hintX, hintY + 2 * Constants::HINT_LINE_SPACING, Constants::TEXT_COLOR);
    renderText(Constants::HINT_EXIT, hintX, hintY + 3 * Constants::HINT_LINE_SPACING, Constants::TEXT_COLOR);

    // Detect mouse hover and show tooltip (only when not dragging a card)
    if (!selectedCard) {
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
    int buttonW = std::max(minW, textW + 10);
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
    int lineHeight = 20;
    int padding = 10;
    
    for (const auto& line : tooltipLines) {
        int textW, textH;
        TTF_SizeUTF8(sdlManager.getFont(), line.c_str(), &textW, &textH);
        maxWidth = std::max(maxWidth, textW);
    }
    
    int tooltipWidth = maxWidth + 2 * padding;
    int tooltipHeight = tooltipLines.size() * lineHeight + 2 * padding;
    
    // Adjust tooltip position to avoid going off screen edges
    int tooltipX = mouseX + 15; // Slight offset to avoid being covered by the mouse
    int tooltipY = mouseY - tooltipHeight / 2;
    
    if (tooltipX + tooltipWidth > Constants::WINDOW_WIDTH) {
        tooltipX = mouseX - tooltipWidth - 15;
    }
    if (tooltipY < 0) {
        tooltipY = 10;
    }
    if (tooltipY + tooltipHeight > Constants::WINDOW_HEIGHT) {
        tooltipY = Constants::WINDOW_HEIGHT - tooltipHeight - 10;
    }
    
    // Render tooltip background
    renderTooltipBackground(tooltipX, tooltipY, tooltipWidth, tooltipHeight);

    // Render text
    for (size_t i = 0; i < tooltipLines.size(); ++i) {
        SDL_Color textColor = (i == 0) ? Constants::TEXT_COLOR : Constants::ATTRIBUTE_TEXT_COLOR;
        renderText(tooltipLines[i], tooltipX + padding, tooltipY + padding + i * lineHeight, textColor);
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