#include <catch2/catch.hpp>

// Forward declare minimal SDL types
struct SDL_Renderer;
struct _TTF_Font; typedef _TTF_Font TTF_Font;

// Minimal SDLManager stub for tests
class SDLManager {
public:
    SDLManager() {}
    ~SDLManager() {}
    SDL_Renderer* getRenderer() const { return nullptr; }
    TTF_Font* getFont() const { return nullptr; }
};

#include "Interface/ui/ContainerCompat.h"
#include "Interface/ui/UIManager.h"
#include "Interface/ui/UIComponent.h"
#include "Interface/ui/UICard.h"
#include "Core/Card.h"
#include <memory>

class MockCard : public UIComponent {
public:
    MockCard(int h, SDLManager& sdl) : UIComponent(0,0,100,h,sdl), clicked(false) {}
    void render() override {}
    void handleEvent(const SDL_Event& e) override { if (e.type==SDL_MOUSEBUTTONDOWN) clicked=true; }
    bool clicked;
};

TEST_CASE("Integration: inventory scroll and tooltip show/hide simulation") {
    SDLManager dummy;
    UIManager mgr;

    // Create inventory using SimpleContainer for modern container functionality
    auto inventoryPtr = std::make_shared<SimpleContainer>(0,0,200,100,dummy);
    inventoryPtr->setScrollable(true); // Enable scrolling like UIContainer had by default

    // Add many cards
    for (int i=0;i<10;++i) {
        auto c = std::make_shared<MockCard>(20,dummy);
        inventoryPtr->addChild(c);
    }

    mgr.addComponent(inventoryPtr, true);

    // Simulate wheel event to scroll down
    SDL_Event wheel{}; wheel.type = SDL_MOUSEWHEEL; wheel.wheel.y = -1; // scroll down
    inventoryPtr->handleEvent(wheel);
    REQUIRE(inventoryPtr->getScrollOffset() > 0);

    // Simulate click at a point that should map to a child
    SDL_Event click{}; click.type = SDL_MOUSEBUTTONDOWN; click.button.x = 10; click.button.y = 10;
    auto c = inventoryPtr->hitTest(10,10);
    REQUIRE(c != nullptr);
    c->handleEvent(click);
}

TEST_CASE("UICard selection functionality") {
    SDLManager dummy;
    
    // Create a test card
    Card testCard("Iron", 1, CardType::METAL, 5);
    UICard uiCard(testCard, 100, 100, dummy);
    
    SECTION("Initial state should be unselected") {
        REQUIRE_FALSE(uiCard.isSelected());
    }
    
    SECTION("Manual selection toggle works") {
        uiCard.setSelected(true);
        REQUIRE(uiCard.isSelected());
        
        uiCard.setSelected(false);
        REQUIRE_FALSE(uiCard.isSelected());
    }
    
    SECTION("Selection state management") {
        // Initially unselected
        REQUIRE_FALSE(uiCard.isSelected());
        
        // Test selecting the card
        uiCard.setSelected(true);
        REQUIRE(uiCard.isSelected());
        
        // Test deselecting the card
        uiCard.setSelected(false);
        REQUIRE_FALSE(uiCard.isSelected());
        
        // Test toggle behavior
        uiCard.setSelected(!uiCard.isSelected());
        REQUIRE(uiCard.isSelected());
        
        uiCard.setSelected(!uiCard.isSelected());
        REQUIRE_FALSE(uiCard.isSelected());
    }
    
    SECTION("Point inside detection works correctly") {
        // Test point inside card bounds
        REQUIRE(uiCard.isPointInside(110, 110)); // Within card bounds (card is at 100,100)
        
        // Test point outside card bounds  
        REQUIRE_FALSE(uiCard.isPointInside(50, 50)); // Outside card bounds
    }
}
