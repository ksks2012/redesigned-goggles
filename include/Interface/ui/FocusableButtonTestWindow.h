#pragma once
#include "Systems/SDLManager.h"
#include "Interface/ui/UIManager.h"
#include "Interface/ui/FocusableButton.h"
#include <memory>
#include <vector>

/**
 * Simple test window for demonstrating FocusableButton functionality
 * Shows multiple buttons with different states and behaviors
 */
class FocusableButtonTestWindow {
public:
    FocusableButtonTestWindow();
    ~FocusableButtonTestWindow();
    
    bool initialize();
    void run();
    void cleanup();

private:
    SDLManager sdlManager_;
    UIManager uiManager_;
    std::vector<std::shared_ptr<FocusableButton>> buttons_;
    bool running_;
    
    void setupButtons();
    void handleEvents();
    void update();
    void render();
    void handleKeyInput(const SDL_Event& event);
    
    // Button callback functions
    void onButton1Click();
    void onButton2Click();
    void onButton3Click();
    void onButton4Click();
};
