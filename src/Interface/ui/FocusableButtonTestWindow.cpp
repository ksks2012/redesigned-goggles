#include "Interface/ui/FocusableButtonTestWindow.h"
#include "Constants.h"
#include <iostream>

FocusableButtonTestWindow::FocusableButtonTestWindow() : running_(false) {
}

FocusableButtonTestWindow::~FocusableButtonTestWindow() {
    cleanup();
}

bool FocusableButtonTestWindow::initialize() {
    // Initialize SDL and create window
    if (!sdlManager_.initialize()) {
        std::cerr << "Failed to initialize SDL!" << std::endl;
        return false;
    }
    
    // Set up buttons
    setupButtons();
    
    running_ = true;
    std::cout << "=== FocusableButton Test Window ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  Tab - Focus next button" << std::endl;
    std::cout << "  Shift+Tab - Focus previous button" << std::endl;
    std::cout << "  Enter/Space - Activate focused button" << std::endl;
    std::cout << "  ESC - Clear focus and exit" << std::endl;
    std::cout << "  Mouse - Click on buttons" << std::endl;
    std::cout << "===============================" << std::endl;
    
    return true;
}

void FocusableButtonTestWindow::setupButtons() {
    // Create multiple buttons with different texts and callbacks
    auto button1 = std::make_shared<FocusableButton>(
        100, 100, 150, 40, sdlManager_, 
        "Hello World", 
        [this]() { onButton1Click(); }
    );
    
    auto button2 = std::make_shared<FocusableButton>(
        100, 160, 150, 40, sdlManager_, 
        "Focus Test", 
        [this]() { onButton2Click(); }
    );
    
    auto button3 = std::make_shared<FocusableButton>(
        300, 100, 150, 40, sdlManager_, 
        "Click Me!", 
        [this]() { onButton3Click(); }
    );
    
    auto button4 = std::make_shared<FocusableButton>(
        300, 160, 150, 40, sdlManager_, 
        "Exit", 
        [this]() { onButton4Click(); }
    );
    
    // Set different z-orders to test layering
    button1->setZOrder(1);
    button2->setZOrder(2);
    button3->setZOrder(3);
    button4->setZOrder(4);
    
    // Add buttons to UI manager and store references
    uiManager_.addComponent(button1);
    uiManager_.addComponent(button2);
    uiManager_.addComponent(button3);
    uiManager_.addComponent(button4);
    
    buttons_.push_back(button1);
    buttons_.push_back(button2);
    buttons_.push_back(button3);
    buttons_.push_back(button4);
    
    // Set initial focus on first button
    uiManager_.setFocus(button1);
}

void FocusableButtonTestWindow::run() {
    while (running_) {
        handleEvents();
        update();
        render();
    }
}

void FocusableButtonTestWindow::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running_ = false;
                break;
                
            case SDL_KEYDOWN:
                handleKeyInput(event);
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                // Handle mouse clicks - find component at mouse position
                {
                    auto clickedComponent = uiManager_.getComponentAt(event.button.x, event.button.y);
                    if (clickedComponent) {
                        // Set focus to clicked component
                        uiManager_.setFocus(clickedComponent);
                        // Let component handle the event
                        clickedComponent->handleEvent(event);
                    }
                }
                break;
                
            default:
                // Forward other events to all components
                for (auto& button : buttons_) {
                    button->handleEvent(event);
                }
                break;
        }
    }
}

void FocusableButtonTestWindow::handleKeyInput(const SDL_Event& event) {
    SDL_Keymod modifiers = SDL_GetModState();
    
    switch (event.key.keysym.sym) {
        case SDLK_TAB:
            if (modifiers & KMOD_SHIFT) {
                uiManager_.focusPrevious();
            } else {
                uiManager_.focusNext();
            }
            break;
            
        case SDLK_ESCAPE:
            uiManager_.clearFocus();
            running_ = false;
            break;
            
        default:
            // Forward key events to focused component
            auto focused = uiManager_.getFocusedComponent();
            if (focused) {
                focused->handleEvent(event);
            }
            break;
    }
}

void FocusableButtonTestWindow::update() {
    // Update any animations or state changes here
    // For now, just layout all components
    uiManager_.layoutAll();
}

void FocusableButtonTestWindow::render() {
    // Clear screen with background color
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 
                          Constants::BACKGROUND_COLOR.r, 
                          Constants::BACKGROUND_COLOR.g, 
                          Constants::BACKGROUND_COLOR.b, 
                          Constants::BACKGROUND_COLOR.a);
    SDL_RenderClear(sdlManager_.getRenderer());
    
    // Render UI title
    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Surface* titleSurface = TTF_RenderUTF8_Solid(
        sdlManager_.getFont(), 
        "FocusableButton Test - Tab/Shift+Tab to navigate, Enter/Space to activate", 
        titleColor
    );
    
    if (titleSurface) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(sdlManager_.getRenderer(), titleSurface);
        if (titleTexture) {
            int titleW, titleH;
            TTF_SizeUTF8(sdlManager_.getFont(), 
                        "FocusableButton Test - Tab/Shift+Tab to navigate, Enter/Space to activate", 
                        &titleW, &titleH);
            SDL_Rect titleRect = {50, 30, titleW, titleH};
            SDL_RenderCopy(sdlManager_.getRenderer(), titleTexture, nullptr, &titleRect);
            SDL_DestroyTexture(titleTexture);
        }
        SDL_FreeSurface(titleSurface);
    }
    
    // Render focus indicator info
    auto focused = uiManager_.getFocusedComponent();
    if (focused) {
        std::string focusInfo = "Currently focused: ";
        for (size_t i = 0; i < buttons_.size(); ++i) {
            if (buttons_[i] == focused) {
                focusInfo += buttons_[i]->getText();
                break;
            }
        }
        
        SDL_Surface* focusSurface = TTF_RenderUTF8_Solid(
            sdlManager_.getFont(), 
            focusInfo.c_str(), 
            Constants::FOCUSED_COLOR
        );
        
        if (focusSurface) {
            SDL_Texture* focusTexture = SDL_CreateTextureFromSurface(sdlManager_.getRenderer(), focusSurface);
            if (focusTexture) {
                int focusW, focusH;
                TTF_SizeUTF8(sdlManager_.getFont(), focusInfo.c_str(), &focusW, &focusH);
                SDL_Rect focusRect = {50, 250, focusW, focusH};
                SDL_RenderCopy(sdlManager_.getRenderer(), focusTexture, nullptr, &focusRect);
                SDL_DestroyTexture(focusTexture);
            }
            SDL_FreeSurface(focusSurface);
        }
    } else {
        SDL_Surface* noFocusSurface = TTF_RenderUTF8_Solid(
            sdlManager_.getFont(), 
            "No component has focus", 
            Constants::SECONDARY_TEXT_COLOR
        );
        
        if (noFocusSurface) {
            SDL_Texture* noFocusTexture = SDL_CreateTextureFromSurface(sdlManager_.getRenderer(), noFocusSurface);
            if (noFocusTexture) {
                int noFocusW, noFocusH;
                TTF_SizeUTF8(sdlManager_.getFont(), "No component has focus", &noFocusW, &noFocusH);
                SDL_Rect noFocusRect = {50, 250, noFocusW, noFocusH};
                SDL_RenderCopy(sdlManager_.getRenderer(), noFocusTexture, nullptr, &noFocusRect);
                SDL_DestroyTexture(noFocusTexture);
            }
            SDL_FreeSurface(noFocusSurface);
        }
    }
    
    // Render all UI components
    uiManager_.renderAll();
    
    // Present the frame
    SDL_RenderPresent(sdlManager_.getRenderer());
}

void FocusableButtonTestWindow::cleanup() {
    buttons_.clear();
    uiManager_.clearPersistent();
    // SDLManager will clean up automatically in its destructor
}

// Button callback implementations
void FocusableButtonTestWindow::onButton1Click() {
    std::cout << "Button 1 (Hello World) clicked!" << std::endl;
}

void FocusableButtonTestWindow::onButton2Click() {
    std::cout << "Button 2 (Focus Test) clicked!" << std::endl;
    // Demonstrate programmatic focus changes
    auto nextButton = (buttons_.size() > 2) ? buttons_[2] : buttons_[0];
    uiManager_.setFocus(nextButton);
    std::cout << "Focus moved to: " << nextButton->getText() << std::endl;
}

void FocusableButtonTestWindow::onButton3Click() {
    std::cout << "Button 3 (Click Me!) clicked!" << std::endl;
    // Demonstrate z-order changes
    static bool onTop = false;
    if (onTop) {
        uiManager_.sendToBottom(buttons_[2]);
        std::cout << "Sent Button 3 to bottom" << std::endl;
    } else {
        uiManager_.bringToTop(buttons_[2]);
        std::cout << "Brought Button 3 to top" << std::endl;
    }
    onTop = !onTop;
}

void FocusableButtonTestWindow::onButton4Click() {
    std::cout << "Exit button clicked! Closing test window..." << std::endl;
    running_ = false;
}
