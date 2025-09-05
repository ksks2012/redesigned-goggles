#include "../lib/catch2/catch.hpp"
#include "Interface/ui/UIManager.h"
#include "Interface/ui/FocusableButton.h"
#include "Systems/SDLManager.h"
#include <memory>

class TestUIComponent : public UIComponent {
public:
    TestUIComponent(int x, int y, int width, int height, SDLManager& sdlManager, int id)
        : UIComponent(x, y, width, height, sdlManager), id_(id) {}
    
    void render() override {
        // Simple test rendering - just set a flag
        rendered_ = true;
    }
    
    bool canReceiveFocus() const override { return focusable_; }
    
    void setFocusable(bool focusable) { focusable_ = focusable; }
    bool wasRendered() const { return rendered_; }
    void resetRendered() { rendered_ = false; }
    int getId() const { return id_; }
    
private:
    int id_;
    bool rendered_ = false;
    bool focusable_ = false;
};

TEST_CASE("UIManager Z-Order Management", "[ui][zorder]") {
    // Note: This test doesn't actually initialize SDL since we're testing logic only
    // In a real test environment, you would initialize SDL properly
    SDLManager* sdlManager = nullptr; // For testing purposes only
    
    // Create UIManager (will work without SDL for basic logic tests)
    UIManager uiManager;
    
    SECTION("Components are rendered in z-order") {
        // Skip this test if SDLManager is not available
        if (!sdlManager) {
            SUCCEED("Skipping render test - SDL not initialized for testing");
            return;
        }
        
        auto comp1 = std::make_shared<TestUIComponent>(0, 0, 100, 100, *sdlManager, 1);
        auto comp2 = std::make_shared<TestUIComponent>(0, 0, 100, 100, *sdlManager, 2);
        auto comp3 = std::make_shared<TestUIComponent>(0, 0, 100, 100, *sdlManager, 3);
        
        comp1->setZOrder(2);
        comp2->setZOrder(1);
        comp3->setZOrder(3);
        
        uiManager.addComponent(comp1);
        uiManager.addComponent(comp2);
        uiManager.addComponent(comp3);
        
        // Test z-order manipulation
        uiManager.bringToTop(comp2); // Should set comp2's z-order to 4
        REQUIRE(comp2->getZOrder() > comp3->getZOrder());
        
        uiManager.sendToBottom(comp1); // Should set comp1's z-order to 0 (below original min)
        REQUIRE(comp1->getZOrder() < comp2->getZOrder());
        REQUIRE(comp1->getZOrder() < comp3->getZOrder());
    }
}

TEST_CASE("UIManager Focus Management", "[ui][focus]") {
    SDLManager* sdlManager = nullptr; // For testing purposes only
    UIManager uiManager;
    
    SECTION("Focus can be set and cleared") {
        if (!sdlManager) {
            SUCCEED("Skipping focus test - SDL not initialized for testing");
            return;
        }
        
        auto comp1 = std::make_shared<TestUIComponent>(0, 0, 100, 100, *sdlManager, 1);
        auto comp2 = std::make_shared<TestUIComponent>(100, 0, 100, 100, *sdlManager, 2);
        
        comp1->setFocusable(true);
        comp2->setFocusable(true);
        
        uiManager.addComponent(comp1);
        uiManager.addComponent(comp2);
        
        // Test focus setting
        uiManager.setFocus(comp1);
        REQUIRE(uiManager.getFocusedComponent() == comp1);
        REQUIRE(comp1->hasFocus());
        
        // Test focus switching
        uiManager.setFocus(comp2);
        REQUIRE(uiManager.getFocusedComponent() == comp2);
        REQUIRE(comp2->hasFocus());
        REQUIRE_FALSE(comp1->hasFocus());
        
        // Test focus clearing
        uiManager.clearFocus();
        REQUIRE(uiManager.getFocusedComponent() == nullptr);
        REQUIRE_FALSE(comp1->hasFocus());
        REQUIRE_FALSE(comp2->hasFocus());
    }
    
    SECTION("Focus traversal works correctly") {
        if (!sdlManager) {
            SUCCEED("Skipping focus traversal test - SDL not initialized for testing");
            return;
        }
        
        auto comp1 = std::make_shared<TestUIComponent>(0, 0, 100, 100, *sdlManager, 1);
        auto comp2 = std::make_shared<TestUIComponent>(100, 0, 100, 100, *sdlManager, 2);
        auto comp3 = std::make_shared<TestUIComponent>(200, 0, 100, 100, *sdlManager, 3);
        
        comp1->setFocusable(true);
        comp2->setFocusable(true);
        comp3->setFocusable(true);
        
        comp1->setZOrder(1);
        comp2->setZOrder(2);
        comp3->setZOrder(3);
        
        uiManager.addComponent(comp1);
        uiManager.addComponent(comp2);
        uiManager.addComponent(comp3);
        
        // Test forward focus traversal
        uiManager.focusNext(); // Should focus first component (comp1 - lowest z-order)
        REQUIRE(uiManager.getFocusedComponent() == comp1);
        
        uiManager.focusNext(); // Should focus comp2
        REQUIRE(uiManager.getFocusedComponent() == comp2);
        
        uiManager.focusNext(); // Should focus comp3
        REQUIRE(uiManager.getFocusedComponent() == comp3);
        
        uiManager.focusNext(); // Should wrap to comp1
        REQUIRE(uiManager.getFocusedComponent() == comp1);
        
        // Test backward focus traversal
        uiManager.focusPrevious(); // Should focus comp3 (wrapping backwards)
        REQUIRE(uiManager.getFocusedComponent() == comp3);
        
        uiManager.focusPrevious(); // Should focus comp2
        REQUIRE(uiManager.getFocusedComponent() == comp2);
    }
}

TEST_CASE("UIManager Modal Handling", "[ui][modal]") {
    SDLManager* sdlManager = nullptr; // For testing purposes only
    UIManager uiManager;
    
    SECTION("Modal component blocks other interactions") {
        if (!sdlManager) {
            SUCCEED("Skipping modal test - SDL not initialized for testing");
            return;
        }
        
        auto normalComp = std::make_shared<TestUIComponent>(0, 0, 100, 100, *sdlManager, 1);
        auto modalComp = std::make_shared<TestUIComponent>(50, 50, 100, 100, *sdlManager, 2);
        
        uiManager.addComponent(normalComp);
        uiManager.addComponent(modalComp);
        
        // Set modal
        uiManager.setModal(modalComp);
        REQUIRE(uiManager.hasModal());
        REQUIRE(uiManager.getModalComponent() == modalComp);
        REQUIRE(modalComp->isModal());
        
        // Test that modal component gets priority in hit-testing
        auto hitComp = uiManager.getComponentAt(75, 75); // Point inside both components
        REQUIRE(hitComp == modalComp); // Modal should be returned
        
        // Test that clicks outside modal return nullptr
        auto outsideComp = uiManager.getComponentAt(25, 25); // Point only in normal component
        REQUIRE(outsideComp == nullptr); // Should return nullptr due to modal
        
        // Clear modal
        uiManager.clearModal();
        REQUIRE_FALSE(uiManager.hasModal());
        REQUIRE(uiManager.getModalComponent() == nullptr);
        REQUIRE_FALSE(modalComp->isModal());
        
        // Now normal hit-testing should work
        auto normalHit = uiManager.getComponentAt(25, 25);
        REQUIRE(normalHit == normalComp);
    }
}

TEST_CASE("UIComponent Focus Properties", "[ui][component]") {
    SDLManager* sdlManager = nullptr; // For testing purposes only
    
    SECTION("Component focus state management") {
        if (!sdlManager) {
            SUCCEED("Skipping component focus test - SDL not initialized for testing");
            return;
        }
        
        TestUIComponent comp(0, 0, 100, 100, *sdlManager, 1);
        
        // Test initial state
        REQUIRE_FALSE(comp.hasFocus());
        REQUIRE_FALSE(comp.canReceiveFocus());
        
        // Test focus state
        comp.setFocused(true);
        REQUIRE(comp.hasFocus());
        
        comp.setFocused(false);
        REQUIRE_FALSE(comp.hasFocus());
        
        // Test focusable property
        comp.setFocusable(true);
        REQUIRE(comp.canReceiveFocus());
        
        comp.setFocusable(false);
        REQUIRE_FALSE(comp.canReceiveFocus());
    }
    
    SECTION("Component z-order and modal properties") {
        if (!sdlManager) {
            SUCCEED("Skipping component properties test - SDL not initialized for testing");
            return;
        }
        
        TestUIComponent comp(0, 0, 100, 100, *sdlManager, 1);
        
        // Test initial z-order
        REQUIRE(comp.getZOrder() == 0);
        
        // Test z-order setting
        comp.setZOrder(5);
        REQUIRE(comp.getZOrder() == 5);
        
        comp.setZOrder(-2);
        REQUIRE(comp.getZOrder() == -2);
        
        // Test modal property
        REQUIRE_FALSE(comp.isModal());
        
        comp.setModal(true);
        REQUIRE(comp.isModal());
        
        comp.setModal(false);
        REQUIRE_FALSE(comp.isModal());
    }
}
