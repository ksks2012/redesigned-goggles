#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "UIComponent.h"

/**
 * Simple UI Manager that holds persistent and dynamic UI components.
 * Responsible for rendering and hit-testing components.
 */
class UIManager {
public:
    UIManager() = default;
    ~UIManager() = default;

    // Add a component. If persistent is true it will remain until clearPersistent()
    void addComponent(std::shared_ptr<UIComponent> comp, bool persistent = true);

    // Clear only dynamic components (useful for per-frame generated UI like lists)
    void clearDynamic();

    // Render all persistent components followed by dynamic components
    void renderAll();

    // Render only components that are fully inside the provided clip rect
    void renderClipped(const SDL_Rect& clip);

    // Find top-most component at a point (dynamic components searched first)
    std::shared_ptr<UIComponent> getComponentAt(int x, int y) const;

private:
    std::vector<std::shared_ptr<UIComponent>> persistent_;
    std::vector<std::shared_ptr<UIComponent>> dynamic_;
};
