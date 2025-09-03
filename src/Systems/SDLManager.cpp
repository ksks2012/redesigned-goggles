#include "Systems/SDLManager.h"
#include <stdexcept>

SDLManager::SDLManager() {
    init();
}

SDLManager::~SDLManager() {
    cleanup();
}

void SDLManager::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
    }
    window = SDL_CreateWindow("Endgame MVP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        throw std::runtime_error("Window creation failed: " + std::string(SDL_GetError()));
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        throw std::runtime_error("Renderer creation failed: " + std::string(SDL_GetError()));
    }
    if (TTF_Init() < 0) {
        throw std::runtime_error("TTF_Init failed: " + std::string(TTF_GetError()));
    }
    font = TTF_OpenFont("./assets/font.ttf", 16);
    if (!font) {
        throw std::runtime_error("Font loading failed: " + std::string(TTF_GetError()));
    }    
}

bool SDLManager::initialize() {
    init();
    return true;
}

void SDLManager::cleanup() {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}