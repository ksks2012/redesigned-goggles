#include "Interface/ui/UIComponent.h"
#include "Systems/SDLManager.h"
#include <iostream>

UIComponent::UIComponent(int x, int y, int width, int height, SDLManager& sdlManager)
    : x_(x), y_(y), width_(width), height_(height), sdlManager_(sdlManager),
      zOrder_(0), isModal_(false), hasFocus_(false) {
}

void UIComponent::renderBackground(SDL_Color color) {
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_Rect rect = {x_, y_, width_, height_};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void UIComponent::renderBorder(SDL_Color color, int thickness) {
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    for (int i = 0; i < thickness; ++i) {
        SDL_Rect borderRect = {x_ - i, y_ - i, width_ + 2 * i, height_ + 2 * i};
        SDL_RenderDrawRect(renderer, &borderRect);
    }
}

void UIComponent::renderText(const std::string& text, int offsetX, int offsetY, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderUTF8_Solid(sdlManager_.getFont(), text.c_str(), color);
    if (!surface) {
        std::cerr << "Text rendering failed: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlManager_.getRenderer(), surface);
    if (!texture) {
        std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    
    int textW, textH;
    TTF_SizeUTF8(sdlManager_.getFont(), text.c_str(), &textW, &textH);
    SDL_Rect dst = {x_ + offsetX, y_ + offsetY, textW, textH};
    SDL_RenderCopy(sdlManager_.getRenderer(), texture, nullptr, &dst);
    
    // RAII cleanup
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

bool UIComponent::isPointInside(int mouseX, int mouseY) const {
    return mouseX >= x_ && mouseX <= x_ + width_ && 
           mouseY >= y_ && mouseY <= y_ + height_;
}

void UIComponent::setPosition(int x, int y) {
    x_ = x;
    y_ = y;
}

void UIComponent::setSize(int width, int height) {
    width_ = width;
    height_ = height;
}

void UIComponent::getTextSize(const std::string& text, int& width, int& height) {
    TTF_SizeUTF8(sdlManager_.getFont(), text.c_str(), &width, &height);
}
