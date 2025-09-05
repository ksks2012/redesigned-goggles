#pragma once
#include "Core/IGameView.h"
#include "Core/Inventory.h"
#include "Systems/CraftingSystem.h"
#include <functional>

/**
 * Interface for handling game input events
 * Follows Command Pattern and Single Responsibility Principle
 */
class IInputHandler {
public:
    virtual ~IInputHandler() = default;
    
    // Event handling methods
    virtual void handleMouseDown(int x, int y) = 0;
    virtual void handleMouseUp(int x, int y) = 0;
    virtual void handleMouseMotion(int x, int y) = 0;
    virtual void handleMouseWheel(int x, int y, int deltaY) = 0;
    virtual void handleKeyDown(int keyCode) = 0;
    virtual void handleQuit() = 0;
};

/**
 * Game input handler implementation
 * Coordinates between input events and game actions
 * Uses View only for UI state queries, not for event processing
 */
class GameInputHandler : public IInputHandler {
public:
    GameInputHandler(IGameView& view, 
                    Inventory& inventory, 
                    CraftingSystem& craftingSystem);
    
    // IInputHandler implementation
    void handleMouseDown(int x, int y) override;
    void handleMouseUp(int x, int y) override;
    void handleMouseMotion(int x, int y) override;
    void handleMouseWheel(int x, int y, int deltaY) override;
    void handleKeyDown(int keyCode) override;
    void handleQuit() override;
    
    // Game state accessors
    bool isRunning() const { return running_; }
    const Card* getSelectedCard() const { return selectedCard_; }
    bool isShowingCraftingPanel() const { return showCraftingPanel_; }
    int getMouseX() const { return mouseX_; }
    int getMouseY() const { return mouseY_; }
    int getInventoryScrollOffset() const { return inventoryScrollOffset_; }
    int getCraftingScrollOffset() const { return craftingScrollOffset_; }
    
    // UICard selection management
    void updateUICardSelection();
    
    // Focus management requests (for Controller to use)
    void requestFocusNext();
    void requestFocusPrevious();
    void requestClearFocus();
    
    // Callback setters for game operations
    void setSaveCallback(std::function<bool()> callback) { saveCallback_ = callback; }
    void setLoadCallback(std::function<bool()> callback) { loadCallback_ = callback; }
    void setExploreCallback(std::function<void()> callback) { exploreCallback_ = callback; }
    
    // Focus management callbacks
    void setFocusNextCallback(std::function<void()> callback) { focusNextCallback_ = callback; }
    void setFocusPreviousCallback(std::function<void()> callback) { focusPreviousCallback_ = callback; }
    void setClearFocusCallback(std::function<void()> callback) { clearFocusCallback_ = callback; }

private:
    IGameView& view_;
    Inventory& inventory_;
    CraftingSystem& craftingSystem_;
    
    // Game state
    bool running_;
    Card* selectedCard_;
    Card* previousSelectedCard_;  // Track previously selected card for UICard state management
    bool showCraftingPanel_;
    int mouseX_, mouseY_;
    
    // Scroll states for different UI areas
    int inventoryScrollOffset_;  // Scroll offset for inventory card list
    int craftingScrollOffset_;   // Scroll offset for crafting panel recipes
    
    // Callbacks for game operations
    std::function<bool()> saveCallback_;
    std::function<bool()> loadCallback_;
    std::function<void()> exploreCallback_;
    
    // Focus management callbacks
    std::function<void()> focusNextCallback_;
    std::function<void()> focusPreviousCallback_;
    std::function<void()> clearFocusCallback_;
    
    // Input handling helpers
    void handleButtonClick(const std::string& buttonName);
    void handleCardClick(const Card* card);
    void handleRecipeClick(int recipeIndex);
    void handleScrollWheel(int x, int y, int deltaY);
    void addRandomCard();
    void removeFirstCard();
    void toggleCraftingPanel();
    void craftRecipe(int recipeIndex);
};
