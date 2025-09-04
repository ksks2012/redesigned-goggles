#include "Interface/ui/UIManager.h"
#include "Systems/SDLManager.h"
#include <algorithm>

void UIManager::addComponent(std::shared_ptr<UIComponent> comp, bool persistent) {
    if (persistent) persistent_.push_back(std::move(comp));
    else dynamic_.push_back(std::move(comp));
}

void UIManager::clearDynamic() {
    dynamic_.clear();
}

void UIManager::renderAll() {
    for (auto& c : persistent_) c->render();
    for (auto& c : dynamic_) c->render();
}

void UIManager::renderClipped(const SDL_Rect& clip) {
    // Render only components fully inside clip rect
    auto renderIfInside = [&](const std::shared_ptr<UIComponent>& c) {
        SDL_Rect r = c->getRect();
        if (r.x >= clip.x && r.x + r.w <= clip.x + clip.w &&
            r.y >= clip.y && r.y + r.h <= clip.y + clip.h) {
            c->render();
        }
    };

    for (auto& c : persistent_) renderIfInside(c);
    for (auto& c : dynamic_) renderIfInside(c);
}

std::shared_ptr<UIComponent> UIManager::getComponentAt(int x, int y) const {
    // Search dynamic first (top-most), then persistent
    auto finder = [&](const std::vector<std::shared_ptr<UIComponent>>& list) -> std::shared_ptr<UIComponent> {
        for (auto it = list.rbegin(); it != list.rend(); ++it) {
            if ((*it)->isPointInside(x, y)) return *it;
        }
        return nullptr;
    };

    auto d = finder(dynamic_);
    if (d) return d;
    return finder(persistent_);
}
