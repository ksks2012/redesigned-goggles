#include <catch2/catch.hpp>

// Forward declare minimal SDL types so tests do not require SDL initialization
struct SDL_Renderer;
struct _TTF_Font; typedef _TTF_Font TTF_Font;

// Minimal SDLManager stub for tests to avoid initializing SDL
class SDLManager {
public:
    SDLManager() {}
    ~SDLManager() {}
    SDL_Renderer* getRenderer() const { return nullptr; }
    TTF_Font* getFont() const { return nullptr; }
};

#include "Interface/ui/UIManager.h"
#include "Interface/ui/UIComponent.h"
#include <memory>

class MockComponent : public UIComponent {
public:
    MockComponent(int x, int y, int w, int h, SDLManager& sdl)
        : UIComponent(x,y,w,h,sdl), clicked(false), hovered(false) {}
    void render() override {}
    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) clicked = true;
        if (event.type == SDL_MOUSEMOTION) hovered = true;
    }
    bool clicked;
    bool hovered;
};

TEST_CASE("UIManager hit-test and event routing") {
    // Create a fake SDLManager via default constructor (not used by test)
    SDLManager dummy;
    UIManager mgr;

    auto a = std::make_shared<MockComponent>(0,0,100,100,dummy);
    auto b = std::make_shared<MockComponent>(10,10,50,50,dummy);

    // Set z-orders to ensure b is on top (higher z-order)
    a->setZOrder(1);
    b->setZOrder(2);

    mgr.addComponent(a, true);
    mgr.addComponent(b, true);

    // Simulate a mouse button event at (20,20) -> should route to topmost component (b)
    SDL_Event ev{};
    ev.type = SDL_MOUSEBUTTONDOWN;
    ev.button.x = 20; ev.button.y = 20;

    auto target = mgr.getComponentAt(20,20);
    REQUIRE(target.get() == b.get());

    // Route event manually (UIManager currently doesn't do event dispatch yet)
    target->handleEvent(ev);
    REQUIRE(b->clicked == true);
    REQUIRE(a->clicked == false);

    // Motion event should set hovered
    SDL_Event mv{}; mv.type = SDL_MOUSEMOTION; mv.motion.x = 20; mv.motion.y = 20;
    target->handleEvent(mv);
    REQUIRE(b->hovered == true);
}
