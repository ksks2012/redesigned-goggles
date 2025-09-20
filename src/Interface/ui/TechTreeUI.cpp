#include "Interface/ui/TechTreeUI.h"
#include <iostream>

TechTreeUI::TechTreeUI(int x, int y, int width, int height, SDLManager& sdlManager, TechTree& tree)
    : UIContainer(x, y, width, height, sdlManager), techTree(tree) {
}

void TechTreeUI::selectTech(const std::string& techId) {
    auto tech = techTree.getTech(techId);
    if (!tech) {
        return;
    }
    
    selectedTech = tech;
    std::cout << "Selected tech: " << tech->name << std::endl;
    std::cout << "Description: " << tech->description << std::endl;
    std::cout << "Status: " << tech->getStatusText() << std::endl;
    std::cout << "Cost: " << tech->researchCost << std::endl;
    
    // Trigger callback
    if (onTechSelected) {
        onTechSelected(techId);
    }
}

void TechTreeUI::updateTechDisplay(const std::string& techId) {
    std::cout << "Updating tech display: " << techId << std::endl;
    
    auto tech = techTree.getTech(techId);
    if (tech) {
        std::cout << "  Status: " << tech->getStatusText() << std::endl;
        std::cout << "  Progress: " << (tech->getProgressPercent() * 100.0f) << "%" << std::endl;
    }
}

void TechTreeUI::refreshTechButtons() {
    std::cout << "Refreshing tech button display" << std::endl;
    
    // This is a simplified version, just print tech list
    const auto& allTechs = techTree.getAllTechs();
    
    std::cout << "=== Tech Tree Status ===" << std::endl;
    for (const auto& pair : allTechs) {
        const auto& tech = pair.second;
        std::cout << "- " << tech->name << " (" << tech->getStatusText() << ")" << std::endl;
    }
    std::cout << "=======================" << std::endl;
}

// TODO:
void TechTreeUI::render() {
    // Render a background for the tech tree UI
    renderBackground({30, 30, 50, 200}); // Dark blue-gray background
    renderBorder({100, 100, 150, 255}, 2); // Light border
    
    // Render title
    renderText("Tech Tree", 10, 10, {255, 255, 255, 255}); // White text
    
    // Render tech list (simplified version)
    const auto& allTechs = techTree.getAllTechs();
    int yOffset = 40;
    int techIndex = 1;
    
    for (const auto& pair : allTechs) {
        const auto& tech = pair.second;
        if (yOffset > height_ - 30) break; // Don't render beyond component bounds
        
        // Choose color based on tech status
        SDL_Color textColor = {180, 180, 180, 255}; // Default gray for LOCKED
        if (tech->status == TechStatus::COMPLETED) {
            textColor = {100, 255, 100, 255}; // Green for completed
        } else if (tech->status == TechStatus::AVAILABLE) {
            textColor = {255, 255, 100, 255}; // Yellow for available
        } else if (tech->status == TechStatus::RESEARCHING) {
            textColor = {255, 150, 100, 255}; // Orange for in progress
        }
        
        // Render tech name with number
        std::string displayText = std::to_string(techIndex) + ". " + tech->name + 
                                 " (" + tech->getStatusText() + ")";
        renderText(displayText, 10, yOffset, textColor);
        
        yOffset += 25;
        techIndex++;
    }
    
    // Render instructions
    renderText("Press 1-3 to select tech, Enter to research", 10, height_ - 60, {200, 200, 200, 255});
    renderText("Press T to close", 10, height_ - 35, {200, 200, 200, 255});
    
    // Display current selected tech info
    if (selectedTech) {
        int infoY = height_ - 120;
        renderText("Selected: " + selectedTech->name, 10, infoY, {255, 255, 0, 255});
        renderText("Cost: " + std::to_string(selectedTech->researchCost), 10, infoY + 20, {255, 255, 0, 255});
    }
}

void TechTreeUI::handleEvent(const SDL_Event& event) {
    // Let parent class handle basic events
    UIContainer::handleEvent(event);
    
    // Simplified event handling: keyboard tech selection
    if (event.type == SDL_KEYDOWN) {
        const auto& allTechs = techTree.getAllTechs();
        auto it = allTechs.begin();
        
        switch (event.key.keysym.sym) {
            case SDLK_1:
                if (allTechs.size() > 0) {
                    selectTech(it->first);
                }
                break;
            case SDLK_2:
                if (allTechs.size() > 1) {
                    std::advance(it, 1);
                    selectTech(it->first);
                }
                break;
            case SDLK_3:
                if (allTechs.size() > 2) {
                    std::advance(it, 2);
                    selectTech(it->first);
                }
                break;
            case SDLK_RETURN:
                // Try to start research on selected tech
                if (selectedTech && onStartResearch) {
                    onStartResearch(selectedTech->id);
                }
                break;
        }
    }
}
