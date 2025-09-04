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
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UIContainer.h"
#include <memory>

// Mock component with layout tracking for testing
class MockLayoutComponent : public UIComponent {
public:
    MockLayoutComponent(int x, int y, int w, int h, SDLManager& sdl)
        : UIComponent(x, y, w, h, sdl), layoutCalled_(false), renderCalled_(false), 
          expectedWidth_(w), expectedHeight_(h) {}
    
    void layout() override {
        layoutCalled_ = true;
        // Simulate layout calculation that might change size
        setSize(expectedWidth_, expectedHeight_);
    }
    
    void render() override {
        renderCalled_ = true;
    }
    
    // Test helpers
    bool wasLayoutCalled() const { return layoutCalled_; }
    bool wasRenderCalled() const { return renderCalled_; }
    void setExpectedSize(int w, int h) { expectedWidth_ = w; expectedHeight_ = h; }
    void reset() { layoutCalled_ = false; renderCalled_ = false; }
    
private:
    bool layoutCalled_;
    bool renderCalled_;
    int expectedWidth_;
    int expectedHeight_;
};

TEST_CASE("UIManager calls layout before render", "[ui][layout]") {
    SDLManager sdl;
    UIManager manager;
    
    auto mock1 = std::make_shared<MockLayoutComponent>(10, 20, 100, 50, sdl);
    auto mock2 = std::make_shared<MockLayoutComponent>(30, 40, 80, 60, sdl);
    
    manager.addComponent(mock1, true);   // persistent
    manager.addComponent(mock2, false);  // dynamic
    
    SECTION("Layout is called before render for all components") {
        manager.renderAll();
        
        REQUIRE(mock1->wasLayoutCalled());
        REQUIRE(mock1->wasRenderCalled());
        REQUIRE(mock2->wasLayoutCalled());
        REQUIRE(mock2->wasRenderCalled());
    }
    
    SECTION("Layout can be called independently") {
        manager.layoutAll();
        
        REQUIRE(mock1->wasLayoutCalled());
        REQUIRE(mock2->wasLayoutCalled());
        REQUIRE_FALSE(mock1->wasRenderCalled());
        REQUIRE_FALSE(mock2->wasRenderCalled());
    }
}

TEST_CASE("UIContainer layout manages child positions", "[ui][layout][container]") {
    SDLManager sdl;
    
    SECTION("Layout positions children vertically") {
        UIContainer container(0, 0, 200, 100, sdl);
        auto child1 = std::make_shared<MockLayoutComponent>(0, 0, 200, 30, sdl);
        auto child2 = std::make_shared<MockLayoutComponent>(0, 0, 200, 25, sdl);
        auto child3 = std::make_shared<MockLayoutComponent>(0, 0, 200, 40, sdl);
        
        container.addChild(child1);
        container.addChild(child2);
        container.addChild(child3);
        
        container.layout();
        
        // Check that layout was called on all children
        REQUIRE(child1->wasLayoutCalled());
        REQUIRE(child2->wasLayoutCalled());
        REQUIRE(child3->wasLayoutCalled());
        
        // Check vertical positioning
        REQUIRE(child1->getY() == 0);      // First child at container top
        REQUIRE(child2->getY() == 30);     // Second child after first (height 30)
        REQUIRE(child3->getY() == 55);     // Third child after second (30 + 25)
        
        // All children should have same X as container
        REQUIRE(child1->getX() == 0);
        REQUIRE(child2->getX() == 0);
        REQUIRE(child3->getX() == 0);
    }
    
    SECTION("Layout with scroll offset adjusts positions") {
        UIContainer container(0, 0, 200, 100, sdl);
        auto child1 = std::make_shared<MockLayoutComponent>(0, 0, 200, 60, sdl);  // Larger children
        auto child2 = std::make_shared<MockLayoutComponent>(0, 0, 200, 50, sdl);
        auto child3 = std::make_shared<MockLayoutComponent>(0, 0, 200, 40, sdl);
        // Total height: 150, container height: 100, so max scroll = 50
        
        container.addChild(child1);
        container.addChild(child2);
        container.addChild(child3);
        
        container.setScrollOffset(20);
        container.layout();
        
        // With scroll offset of 20, all Y positions should be shifted up by 20
        REQUIRE(child1->getY() == -20);    // 0 - 20
        REQUIRE(child2->getY() == 40);     // 60 - 20
        REQUIRE(child3->getY() == 90);     // 110 - 20
    }
}

TEST_CASE("Layout and render separation improves testability", "[ui][layout][testability]") {
    SDLManager sdl;
    
    SECTION("Components can be tested for layout without rendering") {
        auto button = std::make_shared<MockLayoutComponent>(10, 10, 100, 30, sdl);
        button->setExpectedSize(120, 35); // Simulate dynamic sizing
        
        // Test layout calculation independently
        button->layout();
        
        REQUIRE(button->wasLayoutCalled());
        REQUIRE_FALSE(button->wasRenderCalled());
        REQUIRE(button->getWidth() == 120);
        REQUIRE(button->getHeight() == 35);
    }
    
    SECTION("Layout determinism - multiple calls produce same result") {
        UIContainer container(50, 50, 300, 200, sdl);
        auto child = std::make_shared<MockLayoutComponent>(0, 0, 100, 50, sdl);
        container.addChild(child);
        
        // First layout
        container.layout();
        int firstX = child->getX();
        int firstY = child->getY();
        
        // Reset and layout again
        child->reset();
        container.layout();
        int secondX = child->getX();
        int secondY = child->getY();
        
        // Results should be identical
        REQUIRE(firstX == secondX);
        REQUIRE(firstY == secondY);
        REQUIRE(child->wasLayoutCalled()); // Confirm it was called again
    }
}

TEST_CASE("Clipped rendering with layout", "[ui][layout][clipping]") {
    SDLManager sdl;
    UIManager manager;
    
    auto component1 = std::make_shared<MockLayoutComponent>(10, 10, 50, 50, sdl);
    auto component2 = std::make_shared<MockLayoutComponent>(100, 100, 60, 40, sdl);
    auto component3 = std::make_shared<MockLayoutComponent>(200, 200, 70, 30, sdl);
    
    manager.addComponent(component1);
    manager.addComponent(component2);
    manager.addComponent(component3);
    
    SECTION("Layout is called for all components, render only for visible ones") {
        SDL_Rect clipRect = {0, 0, 120, 120}; // Only component1 and part of component2 visible
        
        manager.layoutAndRenderClipped(clipRect);
        
        // Layout should be called for all components
        REQUIRE(component1->wasLayoutCalled());
        REQUIRE(component2->wasLayoutCalled());
        REQUIRE(component3->wasLayoutCalled());
        
        // Render should only be called for components fully inside clip rect
        REQUIRE(component1->wasRenderCalled());  // Fully inside (10,10,50,50) within (0,0,120,120)
        REQUIRE_FALSE(component2->wasRenderCalled()); // Partially outside (100,100,60,40) extends to (160,140)
        REQUIRE_FALSE(component3->wasRenderCalled()); // Completely outside
    }
}
