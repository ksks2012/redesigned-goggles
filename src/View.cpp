#include "View.h"
#include <iostream>

View::View(SDLManager& sdl) : sdlManager(sdl) {}

void View::render(const Inventory& inventory, const Card* selectedCard, int mouseX, int mouseY) {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int index = 0;
    for (const auto& card : inventory.getCards()) {
        int cardY = 50 + index * 60;
        SDL_Rect rect = {50, cardY, 200, 50};
        setRarityColor(card.rarity);
        SDL_RenderFillRect(renderer, &rect);
        std::string text = card.name + " x" + std::to_string(card.quantity);
        renderText(text, 55, cardY + 15, {255, 255, 255, 255});
        index++;
    }

    if (selectedCard) {
        SDL_Rect dragRect = {mouseX - 100, mouseY - 25, 200, 50};
        setRarityColor(selectedCard->rarity);
        SDL_RenderFillRect(renderer, &dragRect);
        std::string text = selectedCard->name + " x" + std::to_string(selectedCard->quantity);
        renderText(text, mouseX - 95, mouseY - 10, {255, 255, 255, 255});
    }

    renderButton("Add Card", 600, 50, 100, 40);
    renderButton("Remove Card", 600, 100, 100, 40);

    SDL_RenderPresent(renderer);
}

void View::setRarityColor(int rarity) {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    switch (rarity) {
        case 1: SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break;
        case 2: SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255); break;
        case 3: SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); break;
    }
}

void View::renderText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(sdlManager.getFont(), text.c_str(), color);
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
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &rect);
    int textX = x + (buttonW - textW) / 2;
    int textY = y + (h - textH) / 2;
    renderText(text, textX, textY, {255, 255, 255, 255});
}