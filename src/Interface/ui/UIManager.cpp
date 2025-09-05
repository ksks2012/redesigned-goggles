#include "Interface/ui/UIManager.h"
#include "Systems/SDLManager.h"
#include <algorithm>

void UIManager::addComponent(std::shared_ptr<UIComponent> comp, bool persistent) {
    if (persistent) {
        persistent_.push_back(comp);
    } else {
        dynamic_.push_back(comp);
    }
}

void UIManager::clearDynamic() {
    // Clear focus if focused component was dynamic
    auto focused = focusedComponent_.lock();
    if (focused) {
        auto it = std::find(dynamic_.begin(), dynamic_.end(), focused);
        if (it != dynamic_.end()) {
            clearFocus();
        }
    }
    
    // Clear modal if modal component was dynamic
    auto modal = modalComponent_.lock();
    if (modal) {
        auto it = std::find(dynamic_.begin(), dynamic_.end(), modal);
        if (it != dynamic_.end()) {
            clearModal();
        }
    }
    
    dynamic_.clear();
}

void UIManager::clearPersistent() {
    // Clear focus if focused component was persistent
    auto focused = focusedComponent_.lock();
    if (focused) {
        auto it = std::find(persistent_.begin(), persistent_.end(), focused);
        if (it != persistent_.end()) {
            clearFocus();
        }
    }
    
    // Clear modal if modal component was persistent
    auto modal = modalComponent_.lock();
    if (modal) {
        auto it = std::find(persistent_.begin(), persistent_.end(), modal);
        if (it != persistent_.end()) {
            clearModal();
        }
    }
    
    persistent_.clear();
}

void UIManager::layoutAll() {
    // Layout persistent components first, then dynamic components
    for (auto& c : persistent_) {
        c->layout();
    }
    for (auto& c : dynamic_) {
        c->layout();
    }
}

void UIManager::renderAll() {
    // Always perform layout before rendering for consistency
    layoutAll();
    
    // Get all components and sort by z-order
    auto allComponents = getAllComponents();
    sortComponentsByZOrder(allComponents);
    
    // Render components in z-order (lowest to highest)
    for (auto& c : allComponents) {
        c->render();
    }
}

void UIManager::layoutAndRenderClipped(const SDL_Rect& clip) {
    // First perform layout for all components
    layoutAll();
    
    // Get all components and sort by z-order
    auto allComponents = getAllComponents();
    sortComponentsByZOrder(allComponents);
    
    // Then render only components fully inside clip rect, in z-order
    for (auto& c : allComponents) {
        SDL_Rect r = c->getRect();
        if (r.x >= clip.x && r.x + r.w <= clip.x + clip.w &&
            r.y >= clip.y && r.y + r.h <= clip.y + clip.h) {
            c->render();
        }
    }
}

std::shared_ptr<UIComponent> UIManager::getComponentAt(int x, int y) const {
    // If there's a modal component, only consider the modal component
    auto modal = modalComponent_.lock();
    if (modal && modal->isPointInside(x, y)) {
        return modal;
    }
    
    // If there's a modal but the point is not inside it, return nullptr
    if (modal) {
        return nullptr;
    }
    
    // Get all components and sort by z-order (highest to lowest for hit-testing)
    auto allComponents = getAllComponents();
    std::sort(allComponents.begin(), allComponents.end(), 
        [](const std::shared_ptr<UIComponent>& a, const std::shared_ptr<UIComponent>& b) {
            return a->getZOrder() > b->getZOrder();
        });
    
    // Find the top-most component at the point
    for (auto& c : allComponents) {
        if (c->isPointInside(x, y)) {
            return c;
        }
    }
    
    return nullptr;
}

// Focus management
void UIManager::setFocus(std::shared_ptr<UIComponent> component) {
    // Clear previous focus
    auto prevFocused = focusedComponent_.lock();
    if (prevFocused) {
        prevFocused->setFocused(false);
        prevFocused->onFocusLost();
    }
    
    // Set new focus if component can receive focus
    if (component && component->canReceiveFocus()) {
        focusedComponent_ = component;
        component->setFocused(true);
        component->onFocusGained();
    } else {
        focusedComponent_.reset();
    }
}

void UIManager::clearFocus() {
    auto focused = focusedComponent_.lock();
    if (focused) {
        focused->setFocused(false);
        focused->onFocusLost();
    }
    focusedComponent_.reset();
}

void UIManager::focusNext() {
    auto focusableComponents = getFocusableComponents();
    if (focusableComponents.empty()) return;
    
    auto current = focusedComponent_.lock();
    if (!current) {
        // No current focus, focus the first component
        setFocus(focusableComponents[0]);
        return;
    }
    
    // Find current component in the list
    auto it = std::find(focusableComponents.begin(), focusableComponents.end(), current);
    if (it != focusableComponents.end()) {
        // Move to next component (wrap around)
        ++it;
        if (it == focusableComponents.end()) {
            it = focusableComponents.begin();
        }
        setFocus(*it);
    } else {
        // Current focus not in list, focus first component
        setFocus(focusableComponents[0]);
    }
}

void UIManager::focusPrevious() {
    auto focusableComponents = getFocusableComponents();
    if (focusableComponents.empty()) return;
    
    auto current = focusedComponent_.lock();
    if (!current) {
        // No current focus, focus the last component
        setFocus(focusableComponents.back());
        return;
    }
    
    // Find current component in the list
    auto it = std::find(focusableComponents.begin(), focusableComponents.end(), current);
    if (it != focusableComponents.end()) {
        // Move to previous component (wrap around)
        if (it == focusableComponents.begin()) {
            it = focusableComponents.end();
        }
        --it;
        setFocus(*it);
    } else {
        // Current focus not in list, focus last component
        setFocus(focusableComponents.back());
    }
}

// Modal handling
void UIManager::setModal(std::shared_ptr<UIComponent> component) {
    if (component) {
        modalComponent_ = component;
        component->setModal(true);
        // Bring modal to top
        bringToTop(component);
    }
}

void UIManager::clearModal() {
    auto modal = modalComponent_.lock();
    if (modal) {
        modal->setModal(false);
    }
    modalComponent_.reset();
}

// Z-order management
void UIManager::bringToTop(std::shared_ptr<UIComponent> component) {
    if (!component) return;
    
    // Find the highest z-order among all components
    int maxZ = 0;
    for (auto& c : persistent_) {
        maxZ = std::max(maxZ, c->getZOrder());
    }
    for (auto& c : dynamic_) {
        maxZ = std::max(maxZ, c->getZOrder());
    }
    
    // Set component z-order to highest + 1
    component->setZOrder(maxZ + 1);
}

void UIManager::sendToBottom(std::shared_ptr<UIComponent> component) {
    if (!component) return;
    
    // Find the lowest z-order among all components
    int minZ = 0;
    for (auto& c : persistent_) {
        minZ = std::min(minZ, c->getZOrder());
    }
    for (auto& c : dynamic_) {
        minZ = std::min(minZ, c->getZOrder());
    }
    
    // Set component z-order to lowest - 1
    component->setZOrder(minZ - 1);
}

// Helper functions
std::vector<std::shared_ptr<UIComponent>> UIManager::getAllComponents() const {
    std::vector<std::shared_ptr<UIComponent>> allComponents;
    allComponents.reserve(persistent_.size() + dynamic_.size());
    
    for (auto& c : persistent_) {
        allComponents.push_back(c);
    }
    for (auto& c : dynamic_) {
        allComponents.push_back(c);
    }
    
    return allComponents;
}

std::vector<std::shared_ptr<UIComponent>> UIManager::getFocusableComponents() const {
    std::vector<std::shared_ptr<UIComponent>> focusable;
    
    for (auto& c : persistent_) {
        if (c->canReceiveFocus()) {
            focusable.push_back(c);
        }
    }
    for (auto& c : dynamic_) {
        if (c->canReceiveFocus()) {
            focusable.push_back(c);
        }
    }
    
    // Sort by z-order for consistent focus traversal
    sortComponentsByZOrder(focusable);
    
    return focusable;
}

void UIManager::sortComponentsByZOrder(std::vector<std::shared_ptr<UIComponent>>& components) const {
    std::sort(components.begin(), components.end(),
        [](const std::shared_ptr<UIComponent>& a, const std::shared_ptr<UIComponent>& b) {
            return a->getZOrder() < b->getZOrder();
        });
}
