#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "SDLManager.h"
#include "Inventory.h"
#include "Card.h"

class View {
public:
    View(SDLManager& sdl);
    void render(const Inventory& inventory, const Card* selectedCard, int mouseX, int mouseY);

private:
    SDLManager& sdlManager;

    void setRarityColor(int rarity);
    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void renderButton(const std::string& text, int x, int y, int minW, int h);
};