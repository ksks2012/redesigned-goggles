#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "UIComponent.h"

/**
 * UIContainer - holds child components in a vertical list with scroll support.
 * Provides clipping and hit-testing that accounts for scroll offset.
 */
class UIContainer : public UIComponent {
public:
    UIContainer(int x, int y, int width, int height, SDLManager& sdl);
    ~UIContainer() override = default;

    void addChild(std::shared_ptr<UIComponent> child);
    void clearChildren();

    void setScrollOffset(int offset);
    int getScrollOffset() const { return scrollOffset_; }
    virtual int getMaxScroll() const;

    // Layout & render separation
    void layout() override;
    void render() override;
    void handleEvent(const SDL_Event& event) override;

    // Hit test takes scroll into account
    std::shared_ptr<UIComponent> hitTest(int x, int y) const;

    // Expose visible count for tests
    int getVisibleCount() const;

private:
    std::vector<std::shared_ptr<UIComponent>> children_;
    int scrollOffset_ = 0; // in pixels
};
