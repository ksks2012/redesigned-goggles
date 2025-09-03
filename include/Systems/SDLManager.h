#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "Constants.h"

class SDLManager {
public:
    SDLManager();
    ~SDLManager();

    bool initialize();
    void cleanup();

    SDL_Window* getWindow() const { return window; }
    SDL_Renderer* getRenderer() const { return renderer; }
    TTF_Font* getFont() const { return font; }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    void init();
};