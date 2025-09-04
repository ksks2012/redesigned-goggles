#include "Interface/ui/UIContainer.h"
#include "Systems/SDLManager.h"
#include <algorithm>

UIContainer::UIContainer(int x, int y, int width, int height, SDLManager& sdl)
    : UIComponent(x, y, width, height, sdl) {
}

void UIContainer::addChild(std::shared_ptr<UIComponent> child) {
    children_.push_back(std::move(child));
}

void UIContainer::clearChildren() {
    children_.clear();
}

void UIContainer::setScrollOffset(int offset) {
    // Clamp to [0, max]
    scrollOffset_ = std::max(0, std::min(offset, getMaxScroll()));
}

int UIContainer::getMaxScroll() const {
    // Sum heights of children minus container height
    int total = 0;
    for (auto& c : children_) total += c->getHeight();
    return std::max(0, total - height_);
}

void UIContainer::layout() {
    // Calculate child positions based on scroll offset
    // This separates layout calculation from rendering
    int offsetY = y_ - scrollOffset_;
    for (auto& c : children_) {
        // First layout the child itself
        c->layout();
        
        // Then position it within this container
        c->setPosition(x_, offsetY);
        offsetY += c->getHeight();
    }
}

void UIContainer::render() {
    // Render children that are fully inside the container bounds
    // Layout should already be calculated by this point
    SDL_Rect clip = {x_, y_, width_, height_};
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_RenderSetClipRect(renderer, &clip);

    for (auto& c : children_) {
        SDL_Rect r = c->getRect();
        // Only render if child is visible within container bounds
        if (r.y + r.h >= y_ && r.y <= y_ + height_) {
            c->render();
        }
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}

void UIContainer::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEWHEEL) {
        // Scroll by wheel y * some constant (assume 10 pixels per wheel unit)
        setScrollOffset(scrollOffset_ - event.wheel.y * 10);
    }
}

std::shared_ptr<UIComponent> UIContainer::hitTest(int x, int y) const {
    if (x < x_ || x > x_ + width_ || y < y_ || y > y_ + height_) return nullptr;
    int localY = y + scrollOffset_ - y_;
    int offsetY = 0;
    for (auto it = children_.begin(); it != children_.end(); ++it) {
        auto& c = *it;
        if (localY >= offsetY && localY <= offsetY + c->getHeight()) return c;
        offsetY += c->getHeight();
    }
    return nullptr;
}

int UIContainer::getVisibleCount() const {
    int count = 0;
    int offsetY = -scrollOffset_;
    for (auto& c : children_) {
        if (offsetY + c->getHeight() >= 0 && offsetY <= height_) ++count;
        offsetY += c->getHeight();
    }
    return count;
}
