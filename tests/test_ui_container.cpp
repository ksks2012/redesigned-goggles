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

#include "Interface/ui/UIContainer.h"
#include "Interface/ui/UIComponent.h"
#include <memory>

class SimpleChild : public UIComponent {
public:
    SimpleChild(int w, int h, SDLManager& sdl) : UIComponent(0,0,w,h,sdl) {}
    void render() override {}
};

TEST_CASE("UIContainer scroll and clipping") {
    SDLManager dummy;
    UIContainer container(0,0,200,100,dummy);

    // add five children, each 30px tall
    for (int i = 0; i < 5; ++i) {
        auto c = std::make_shared<SimpleChild>(200,30,dummy);
        container.addChild(c);
    }

    REQUIRE(container.getMaxScroll() == std::max(0, 5*30 - 100));
    REQUIRE(container.getVisibleCount() == 4); // 100px container -> 4 visible due to partial overlap

    container.setScrollOffset(40);
    REQUIRE(container.getScrollOffset() == 40);
    // hitTest at y in second child's visible area
    auto hit = container.hitTest(10, 25); // with scroll 40, this maps to localY = 25+40 = 65 -> child index 2
    REQUIRE(hit != nullptr);
}
