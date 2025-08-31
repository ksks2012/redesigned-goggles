#include "View.h"
#include "Constants.h"
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
        std::string text = card.name + " x" + std::to_string(card.quantity);
        renderText(text, Constants::CARD_X + Constants::CARD_TEXT_OFFSET_X, cardY + Constants::CARD_TEXT_OFFSET_Y,
                   Constants::TEXT_COLOR);
        index++;
    }

    if (selectedCard) {
        SDL_Rect dragRect = {mouseX + Constants::DRAG_CARD_OFFSET_X, mouseY + Constants::DRAG_CARD_OFFSET_Y,
                             Constants::CARD_WIDTH, Constants::CARD_HEIGHT};
        setRarityColor(selectedCard->rarity);
        SDL_RenderFillRect(renderer, &dragRect);
        std::string text = selectedCard->name + " x" + std::to_string(selectedCard->quantity);
        renderText(text, mouseX + Constants::DRAG_TEXT_OFFSET_X, mouseY + Constants::DRAG_TEXT_OFFSET_Y,
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