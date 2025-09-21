#pragma once
#include "Interface/ui/UIContainer.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/TechTree.h"
#include "Systems/SDLManager.h"
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <map>

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
     * Layout UI components
     * Override to use UILayoutContainer's absolute layout instead of UIContainer's vertical layout
     */
    void layout() override;

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
    
    // UILabel components for each tech node
    std::map<std::string, std::shared_ptr<UILabel>> techLabels;  ///< UILabel for each tech node
    
    // Layout and rendering helpers
    void createTechLabels();                     ///< Create UILabel components for all tech nodes
    void updateTechLabelColors();                ///< Update colors based on tech status
    void renderConnections();                    ///< Render connection lines between tech nodes
    void calculateTechLayout();                  ///< Calculate optimal positions for tech nodes
    SDL_Color getTechStatusColor(TechStatus status); ///< Get color based on tech status
    
    // Layout calculation helpers
    void calculateHierarchicalLayout();          ///< Calculate positions based on dependency hierarchy
    int calculateTechLevel(const std::string& techId, std::map<std::string, int>& levelCache); ///< Calculate tech tree level
    void positionTechsInLevel(const std::vector<std::string>& techsInLevel, int level); ///< Position techs within a level
    void positionTechsWithoutOverlap(const std::vector<std::string>& techIds, int level, int levelY, int marginX); ///< Position techs preventing overlaps
    
    // Mouse interaction
    std::string getTechAtPosition(int x, int y); ///< Get tech ID at given position
};
