#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>
#include "Constants.h"

// Forward declaration
class SDLManager;

/**
 * Abstract base class for all UI components
 * Provides common functionality for rendering backgrounds, borders, and text
 * Uses RAII for resource management
 */
class UIComponent {
public:
    UIComponent(int x, int y, int width, int height, SDLManager& sdlManager);
    virtual ~UIComponent() = default;

    // Pure virtual function for rendering - must be implemented by derived classes
    virtual void render() = 0;
    
    // Common rendering functions
    void renderBackground(SDL_Color color);
    void renderBorder(SDL_Color color, int thickness = 1);
    void renderText(const std::string& text, int offsetX, int offsetY, SDL_Color color);
    
    // Utility functions
    bool isPointInside(int mouseX, int mouseY) const;
    void setPosition(int x, int y);
    void setSize(int width, int height);
    
    // Getters
    int getX() const { return x_; }
    int getY() const { return y_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    SDL_Rect getRect() const { return {x_, y_, width_, height_}; }

protected:
    int x_, y_, width_, height_;
    SDLManager& sdlManager_;
    
    // Helper function to get text dimensions
    void getTextSize(const std::string& text, int& width, int& height);
};