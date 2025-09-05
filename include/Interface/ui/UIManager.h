#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "UIComponent.h"

/**
 * Simple UI Manager that holds persistent and dynamic UI components.
 * Responsible for layout calculation, rendering and hit-testing components.
 * Separates layout phase from rendering phase for better performance and testability.
 * Supports z-order rendering, modal dialogs, and keyboard focus management.
 */
class UIManager {
public:
    UIManager() = default;
    ~UIManager() = default;

    // Add a component. If persistent is true it will remain until clearPersistent()
    void addComponent(std::shared_ptr<UIComponent> comp, bool persistent = true);

    // Clear only dynamic components (useful for per-frame generated UI like lists)
    void clearDynamic();
    
    // Clear persistent components
    void clearPersistent();

    // Layout all components (calculates positions and sizes)
    void layoutAll();

    // Render all persistent components followed by dynamic components
    // Automatically calls layoutAll() before rendering
    // Renders in z-order (lowest to highest)
    void renderAll();

    // Layout and render only components that are fully inside the provided clip rect
    void layoutAndRenderClipped(const SDL_Rect& clip);

    // Find top-most component at a point (considers z-order and modal state)
    std::shared_ptr<UIComponent> getComponentAt(int x, int y) const;
    
    // Focus management
    void setFocus(std::shared_ptr<UIComponent> component);
    void clearFocus();
    std::shared_ptr<UIComponent> getFocusedComponent() const { return focusedComponent_.lock(); }
    void focusNext(); // Move focus to next focusable component
    void focusPrevious(); // Move focus to previous focusable component
    
    // Modal handling
    void setModal(std::shared_ptr<UIComponent> component);
    void clearModal();
    std::shared_ptr<UIComponent> getModalComponent() const { return modalComponent_.lock(); }
    bool hasModal() const { return !modalComponent_.expired(); }
    
    // Z-order management
    void bringToTop(std::shared_ptr<UIComponent> component);
    void sendToBottom(std::shared_ptr<UIComponent> component);

private:
    std::vector<std::shared_ptr<UIComponent>> persistent_;
    std::vector<std::shared_ptr<UIComponent>> dynamic_;
    
    // Focus and modal management
    std::weak_ptr<UIComponent> focusedComponent_;
    std::weak_ptr<UIComponent> modalComponent_;
    
    // Helper functions
    std::vector<std::shared_ptr<UIComponent>> getAllComponents() const;
    std::vector<std::shared_ptr<UIComponent>> getFocusableComponents() const;
    void sortComponentsByZOrder(std::vector<std::shared_ptr<UIComponent>>& components) const;
};
