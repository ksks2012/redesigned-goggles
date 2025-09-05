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
 * Separates layout calculation from rendering for better testability and determinism
 * Supports z-order, modal, and focus handling
 */
class UIComponent {
public:
    UIComponent(int x, int y, int width, int height, SDLManager& sdlManager);
    virtual ~UIComponent() = default;

    // Layout and rendering separation
    virtual void layout() {} // Optional layout calculation - implement if component needs layout logic
    virtual void render() = 0; // Pure virtual function for rendering - must be implemented by derived classes
    
    // Optional event handler for components (mouse/keyboard/scroll)
    virtual void handleEvent(const SDL_Event& event) {}
    
    // Focus handling
    virtual void onFocusGained() {} // Called when component gains focus
    virtual void onFocusLost() {} // Called when component loses focus
    virtual bool canReceiveFocus() const { return false; } // Override to true for focusable components
    
    // Common rendering functions
    void renderBackground(SDL_Color color);
    void renderBorder(SDL_Color color, int thickness = 1);
    void renderText(const std::string& text, int offsetX, int offsetY, SDL_Color color);
    
    // Utility functions
    bool isPointInside(int mouseX, int mouseY) const;
    void setPosition(int x, int y);
    void setSize(int width, int height);
    
    // Z-order and modal properties
    void setZOrder(int zOrder) { zOrder_ = zOrder; }
    int getZOrder() const { return zOrder_; }
    void setModal(bool modal) { isModal_ = modal; }
    bool isModal() const { return isModal_; }
    
    // Focus properties
    void setFocused(bool focused) { hasFocus_ = focused; }
    bool hasFocus() const { return hasFocus_; }
    
    // Getters
    int getX() const { return x_; }
    int getY() const { return y_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    SDL_Rect getRect() const { return {x_, y_, width_, height_}; }

protected:
    int x_, y_, width_, height_;
    SDLManager& sdlManager_;
    
    // Z-order, modal, and focus state
    int zOrder_ = 0;
    bool isModal_ = false;
    bool hasFocus_ = false;
    
    // Helper function to get text dimensions
    void getTextSize(const std::string& text, int& width, int& height);
};