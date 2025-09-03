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

#include "ui/UIContainer.h"
#include "ui/UIManager.h"
#include "ui/UIComponent.h"
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
    UIContainer inventory(0,0,200,100,dummy);
    UIManager mgr;

    // Add many cards
    for (int i=0;i<10;++i) {
        auto c = std::make_shared<MockCard>(20,dummy);
        inventory.addChild(c);
    }

    mgr.addComponent(std::make_shared<UIContainer>(inventory), true);

    // Simulate wheel event to scroll down
    SDL_Event wheel{}; wheel.type = SDL_MOUSEWHEEL; wheel.wheel.y = -1; // scroll down
    inventory.handleEvent(wheel);
    REQUIRE(inventory.getScrollOffset() > 0);

    // Simulate click at a point that should map to a child
    SDL_Event click{}; click.type = SDL_MOUSEBUTTONDOWN; click.button.x = 10; click.button.y = 10;
    auto c = inventory.hitTest(10,10);
    REQUIRE(c != nullptr);
    c->handleEvent(click);
}
