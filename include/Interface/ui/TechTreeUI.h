#pragma once
#include "Interface/ui/UIContainer.h"
#include "Interface/ui/TechTree.h"
#include "Systems/SDLManager.h"
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

/**
 * Tech Tree UI Component (Simplified Version)
 * Provides basic display and interaction functionality for the tech tree
 */
class TechTreeUI : public UIContainer {
public:
    /**
     * Constructor
     * @param x Position X coordinate
     * @param y Position Y coordinate
     * @param width Width
     * @param height Height
     * @param sdlManager SDL manager reference
     * @param tree Tech tree reference
     */
    TechTreeUI(int x, int y, int width, int height, SDLManager& sdlManager, TechTree& tree);

    /**
     * Select specified technology
     * @param techId Technology ID
     */
    void selectTech(const std::string& techId);

    /**
     * Update display for specified technology
     * @param techId Technology ID
     */
    void updateTechDisplay(const std::string& techId);

    /**
     * Refresh technology button display
     */
    void refreshTechButtons();

    /**
     * Render UI
     */
    void render() override;

    /**
     * Handle events
     * @param event SDL event
     */
    void handleEvent(const SDL_Event& event) override;

    // Callback function settings
    std::function<void(const std::string&)> onTechSelected;        ///< Tech selection callback
    std::function<bool(const std::string&)> onStartResearch;       ///< Start research callback

private:
    TechTree& techTree;                          ///< Tech tree reference
    std::shared_ptr<TechNode> selectedTech;      ///< Currently selected technology
};
