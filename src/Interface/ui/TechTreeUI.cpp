#include "Interface/ui/TechTreeUI.h"
#include "Interface/ui/UILabel.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>

TechTreeUI::TechTreeUI(int x, int y, int width, int height, SDLManager& sdlManager, TechTree& tree)
    : UIContainer(x, y, width, height, sdlManager), techTree(tree) {
    
    // Set absolute layout to preserve our calculated tech node positions
    setAbsoluteLayout();
    
    createTechLabels();
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
    
    // Recreate UILabel components to reflect any changes in tech tree
    createTechLabels();
    
    // Print tech list for debugging
    const auto& allTechs = techTree.getAllTechs();
    
    std::cout << "=== Tech Tree Status ===" << std::endl;
    for (const auto& pair : allTechs) {
        const auto& tech = pair.second;
        std::cout << "- " << tech->name << " (" << tech->getStatusText() << ")" 
                  << " at (" << tech->x << ", " << tech->y << ")" << std::endl;
    }
    std::cout << "=======================" << std::endl;
    
    // Update the display to reflect the new layout
    updateTechLabelColors();
}

void TechTreeUI::layout() {
    // Use UILayoutContainer's layout system instead of UIContainer's vertical layout
    UILayoutContainer::layout();
}

// Updated render method using UILabel components
void TechTreeUI::render() {
    // Render background
    renderBackground({30, 30, 50, 200}); // Dark blue-gray background
    renderBorder({100, 100, 150, 255}, 2); // Light border
    
    // Update tech label colors based on current status
    updateTechLabelColors();
    
    // Render connection lines first (so they appear behind the tech nodes)
    renderConnections();
    
    // Render all UILabel components (tech nodes)
    UIContainer::render(); // This will render all child components including UILabels
    
    // Render title and instructions on top
    renderText("Tech Tree", 10, 10, {255, 255, 255, 255}); // White text
    
    // Render instructions at the bottom
    renderText("Click on tech nodes to select, Enter to research", 10, height_ - 60, {200, 200, 200, 255});
    renderText("Use 1-3 keys for quick selection, T to close", 10, height_ - 35, {200, 200, 200, 255});
    
    // Display current selected tech info
    if (selectedTech) {
        int infoY = height_ - 120;
        renderText("Selected: " + selectedTech->name, 10, infoY, {255, 255, 0, 255});
        renderText("Cost: " + std::to_string(selectedTech->researchCost), 10, infoY + 20, {255, 255, 0, 255});
        renderText("Status: " + selectedTech->getStatusText(), 10, infoY + 40, {255, 255, 0, 255});
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
    
    // Check for mouse clicks on tech nodes
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mouseX = event.button.x - x_;
        int mouseY = event.button.y - y_;
        std::string clickedTech = getTechAtPosition(mouseX, mouseY);
        if (!clickedTech.empty()) {
            selectTech(clickedTech);
        }
    }
}

void TechTreeUI::createTechLabels() {
    // Clear existing labels first
    techLabels.clear();
    clearChildren(); // Remove all child components
    
    // First, calculate optimal positions for all tech nodes
    calculateTechLayout();
    
    const auto& allTechs = techTree.getAllTechs();
    
    for (const auto& pair : allTechs) {
        const auto& tech = pair.second;
        
        // Create UILabel for this tech node
        auto label = std::make_shared<UILabel>(
            0, 0, tech->width, tech->height,
            sdlManager_, tech->name
        );
        
        // Set label properties
        label->setTextAlignment(UILabel::TextAlignment::CENTER);
        label->setBackgroundColor({50, 50, 80, 200});  // Semi-transparent blue background
        label->setWordWrap(true);
        
        // Set initial color based on tech status
        label->setTextColor(getTechStatusColor(tech->status));
        
        techLabels[tech->id] = label;
        
        // Add to absolute layout with calculated position
        setAbsolutePosition(label, {tech->x, tech->y, tech->width, tech->height});
    }
    
    // Trigger UILayoutContainer's layout to apply absolute positions
    UILayoutContainer::layout();
}

void TechTreeUI::updateTechLabelColors() {
    const auto& allTechs = techTree.getAllTechs();
    
    for (const auto& pair : allTechs) {
        const auto& tech = pair.second;
        auto labelIt = techLabels.find(tech->id);
        
        if (labelIt != techLabels.end()) {
            SDL_Color color = getTechStatusColor(tech->status);
            
            // Highlight selected tech
            if (selectedTech && selectedTech->id == tech->id) {
                color = {255, 255, 0, 255}; // Yellow for selected
                labelIt->second->setBackgroundColor({100, 100, 0, 200}); // Dark yellow background
            } else {
                labelIt->second->setBackgroundColor({50, 50, 80, 200}); // Default background
            }
            
            labelIt->second->setTextColor(color);
        }
    }
}

void TechTreeUI::renderConnections() {
    const auto& allTechs = techTree.getAllTechs();
    
    for (const auto& pair : allTechs) {
        const auto& tech = pair.second;
        
        // Draw lines to all prerequisites
        for (const auto& prereq : tech->prerequisites) {
            auto prereqTech = techTree.getTech(prereq.techId);
            if (prereqTech) {
                // Use UILabel positions for more accurate connections
                auto fromLabelIt = techLabels.find(prereq.techId);
                auto toLabelIt = techLabels.find(tech->id);
                
                int fromX, fromY, toX, toY;
                
                if (fromLabelIt != techLabels.end() && toLabelIt != techLabels.end()) {
                    // Use actual UILabel positions (more accurate)
                    auto fromLabel = fromLabelIt->second;
                    auto toLabel = toLabelIt->second;
                    fromX = fromLabel->getX() + fromLabel->getWidth() / 2;
                    fromY = fromLabel->getY() + fromLabel->getHeight() / 2;
                    toX = toLabel->getX() + toLabel->getWidth() / 2;
                    toY = toLabel->getY() + toLabel->getHeight() / 2;
                } else {
                    // Fallback to TechNode positions with container offset
                    fromX = prereqTech->x + prereqTech->width / 2;
                    fromY = prereqTech->y + prereqTech->height / 2;
                    toX = tech->x + tech->width / 2;
                    toY = tech->y + tech->height / 2;
                }
                
                // Choose line color based on prerequisite status
                SDL_Color lineColor;
                if (prereqTech->status == TechStatus::COMPLETED) {
                    lineColor = {100, 255, 100, 255}; // Green for completed
                } else if (prereqTech->status == TechStatus::AVAILABLE || 
                          prereqTech->status == TechStatus::RESEARCHING) {
                    lineColor = {255, 255, 100, 255}; // Yellow for available/researching
                } else {
                    lineColor = {128, 128, 128, 255}; // Gray for locked
                }
                
                // Draw line using SDL
                SDL_Renderer* renderer = sdlManager_.getRenderer();
                SDL_SetRenderDrawColor(renderer, lineColor.r, lineColor.g, lineColor.b, lineColor.a);
                SDL_RenderDrawLine(renderer, fromX, fromY, toX, toY);
                
                // Draw arrow head (simple triangle)
                int dx = toX - fromX;
                int dy = toY - fromY;
                float length = sqrt(dx * dx + dy * dy);
                if (length > 10) { // Only draw arrow if connection is long enough
                    // Normalize direction
                    float ndx = dx / length;
                    float ndy = dy / length;
                    
                    // Calculate arrow head points
                    int arrowSize = 8; // Slightly larger arrow
                    int headX = toX - (int)(ndx * arrowSize);
                    int headY = toY - (int)(ndy * arrowSize);
                    
                    // Draw simple arrow lines
                    int perpX = (int)(-ndy * arrowSize / 2);
                    int perpY = (int)(ndx * arrowSize / 2);
                    
                    SDL_RenderDrawLine(renderer, toX, toY, headX + perpX, headY + perpY);
                    SDL_RenderDrawLine(renderer, toX, toY, headX - perpX, headY - perpY);
                }
            }
        }
    }
}

SDL_Color TechTreeUI::getTechStatusColor(TechStatus status) {
    switch (status) {
        case TechStatus::COMPLETED:
            return {100, 255, 100, 255}; // Green
        case TechStatus::AVAILABLE:
            return {255, 255, 100, 255}; // Yellow
        case TechStatus::RESEARCHING:
            return {255, 150, 100, 255}; // Orange
        case TechStatus::LOCKED:
        default:
            return {180, 180, 180, 255}; // Gray
    }
}

std::string TechTreeUI::getTechAtPosition(int x, int y) {
    for (const auto& pair : techLabels) {
        const auto& label = pair.second;
        
        // Check if point is within label bounds
        if (x >= label->getX() && x <= label->getX() + label->getWidth() &&
            y >= label->getY() && y <= label->getY() + label->getHeight()) {
            return pair.first; // Return tech ID
        }
    }
    return ""; // No tech found at position
}

void TechTreeUI::calculateTechLayout() {
    std::cout << "Calculating automatic tech layout..." << std::endl;
    calculateHierarchicalLayout();
}

void TechTreeUI::calculateHierarchicalLayout() {
    const auto& allTechs = techTree.getAllTechs();
    if (allTechs.empty()) {
        return;
    }
    
    std::cout << "Layout parameters: width=" << width_ << ", height=" << height_;
    
    // Step 1: Calculate the level (depth) of each tech in the dependency tree
    std::map<std::string, int> levelCache;
    std::map<int, std::vector<std::string>> techsByLevel;
    int maxLevel = 0;
    
    for (const auto& pair : allTechs) {
        const std::string& techId = pair.first;
        int level = calculateTechLevel(techId, levelCache);
        techsByLevel[level].push_back(techId);
        maxLevel = std::max(maxLevel, level);
    }
    
    // Step 2: Calculate adaptive spacing based on content
    const int minLevelSpacing = 140; // Minimum vertical spacing between levels
    const int maxLevelSpacing = 200; // Maximum vertical spacing
    const int startY = 50;           // Starting Y position
    const int marginX = 60;          // Left/right margin
    
    // Adaptive level spacing based on available height
    int availableHeight = height_ - (2 * startY);
    int levelSpacing = maxLevel > 0 ? 
        std::min(maxLevelSpacing, std::max(minLevelSpacing, availableHeight / (maxLevel + 1))) :
        maxLevelSpacing;
    
    std::cout << ", levels=" << (maxLevel + 1) << ", spacing=" << levelSpacing << std::endl;
    
    // Step 3: Position techs within each level with overlap prevention
    for (const auto& levelPair : techsByLevel) {
        int level = levelPair.first;
        const auto& techsInLevel = levelPair.second;
        
        // Calculate Y position for this level
        int levelY = startY + level * levelSpacing;
        
        if (techsInLevel.size() == 1) {
            // Single tech - center it
            const std::string& techId = techsInLevel[0];
            auto tech = techTree.getTech(techId);
            if (tech) {
                int availableWidth = width_ - (2 * marginX);
                tech->x = marginX + (availableWidth - tech->width) / 2;
                tech->y = levelY;
                std::cout << "Positioned " << tech->name << " at level " << level 
                         << " (" << tech->x << ", " << levelY << ") [1/1]" << std::endl;
            }
        } else {
            // Multiple techs - use anti-overlap algorithm
            positionTechsWithoutOverlap(techsInLevel, level, levelY, marginX);
        }
    }
}

void TechTreeUI::positionTechsWithoutOverlap(const std::vector<std::string>& techIds, 
                                           int level, int levelY, int marginX) {
    if (techIds.empty()) return;
    
    struct TechInfo {
        std::string id;
        std::shared_ptr<TechNode> tech;
        int width;
        int desiredX;  // Initial desired position
        int finalX;    // Final collision-free position
    };
    
    std::vector<TechInfo> techs;
    
    // Step 1: Collect tech info and calculate initial desired positions
    int availableWidth = width_ - (2 * marginX);
    const int minGap = 10; // Minimum gap between nodes
    const int nodeWidth = 120; // Assume standard node width
    
    // Calculate required space
    int requiredWidth = (int)techIds.size() * nodeWidth + ((int)techIds.size() - 1) * minGap;
    
    std::cout << "Level " << level << ": Available=" << availableWidth 
              << "px, Required=" << requiredWidth << "px for " << techIds.size() << " nodes" << std::endl;
    
    if (requiredWidth > availableWidth) {
        // Not enough space - use compact layout
        std::cout << "Using compact layout due to space constraints" << std::endl;
        int compactSpacing = availableWidth / ((int)techIds.size() + 1);
        
        for (size_t i = 0; i < techIds.size(); ++i) {
            auto tech = techTree.getTech(techIds[i]);
            if (tech) {
                TechInfo info;
                info.id = techIds[i];
                info.tech = tech;
                info.width = tech->width;
                info.finalX = marginX + (int)(i + 1) * compactSpacing - tech->width / 2;
                
                // Ensure it stays within bounds
                int minX = marginX;
                int maxX = width_ - info.width - marginX;
                info.finalX = std::max(minX, std::min(info.finalX, maxX));
                
                techs.push_back(info);
            }
        }
    } else {
        // Enough space - use normal layout with proper spacing
        int spacingBetweenCenters = availableWidth / ((int)techIds.size() + 1);
        
        for (size_t i = 0; i < techIds.size(); ++i) {
            auto tech = techTree.getTech(techIds[i]);
            if (tech) {
                TechInfo info;
                info.id = techIds[i];
                info.tech = tech;
                info.width = tech->width;
                info.desiredX = marginX + (int)(i + 1) * spacingBetweenCenters - tech->width / 2;
                info.finalX = info.desiredX;
                techs.push_back(info);
            }
        }
        
        // Fine-tune positions to avoid overlaps
        std::sort(techs.begin(), techs.end(), 
                 [](const TechInfo& a, const TechInfo& b) { 
                     return a.finalX < b.finalX; 
                 });
        
        // Adjust overlapping nodes
        for (size_t i = 0; i < techs.size() - 1; ++i) {
            int leftRight = techs[i].finalX + techs[i].width;
            int rightLeft = techs[i + 1].finalX;
            int gap = rightLeft - leftRight;
            
            if (gap < minGap) {
                int adjustment = (minGap - gap + 1) / 2; // Split the adjustment
                techs[i].finalX -= adjustment;
                techs[i + 1].finalX += adjustment;
                
                // Keep in bounds
                int minX = marginX;
                int maxX = width_ - techs[i].width - marginX;
                techs[i].finalX = std::max(minX, std::min(techs[i].finalX, maxX));
                
                maxX = width_ - techs[i + 1].width - marginX;
                techs[i + 1].finalX = std::max(minX, std::min(techs[i + 1].finalX, maxX));
            }
        }
    }
    
    // Step 2: Apply final positions
    for (const auto& info : techs) {
        info.tech->x = info.finalX;
        info.tech->y = levelY;
        
        std::cout << "Final position: " << info.tech->name << " at level " << level 
                 << " (" << info.finalX << ", " << levelY << ") width=" << info.width << std::endl;
    }
    
    // Step 3: Verify final gaps (for logging only)
    std::sort(techs.begin(), techs.end(), 
             [](const TechInfo& a, const TechInfo& b) { 
                 return a.finalX < b.finalX; 
             });
             
    for (size_t i = 0; i < techs.size() - 1; ++i) {
        int gap = techs[i + 1].finalX - (techs[i].finalX + techs[i].width);
        std::cout << "Gap between " << techs[i].tech->name << " and " 
                 << techs[i + 1].tech->name << ": " << gap << "px" << std::endl;
    }
}

int TechTreeUI::calculateTechLevel(const std::string& techId, std::map<std::string, int>& levelCache) {
    // Check if level already calculated
    auto it = levelCache.find(techId);
    if (it != levelCache.end()) {
        return it->second;
    }
    
    auto tech = techTree.getTech(techId);
    if (!tech) {
        levelCache[techId] = 0;
        return 0;
    }
    
    // If tech has no prerequisites, it's at level 0
    if (tech->prerequisites.empty()) {
        levelCache[techId] = 0;
        return 0;
    }
    
    // Calculate level based on maximum prerequisite level + 1
    int maxPrereqLevel = -1;
    for (const auto& prereq : tech->prerequisites) {
        int prereqLevel = calculateTechLevel(prereq.techId, levelCache);
        maxPrereqLevel = std::max(maxPrereqLevel, prereqLevel);
    }
    
    int level = maxPrereqLevel + 1;
    levelCache[techId] = level;
    return level;
}

void TechTreeUI::positionTechsInLevel(const std::vector<std::string>& techsInLevel, int level) {
    // This method can be used for more sophisticated positioning within a level
    // For now, the logic is implemented in calculateHierarchicalLayout()
    // But this provides a place for future enhancements like:
    // - Grouping by tech type
    // - Avoiding overlaps with connection lines
    // - Custom spacing rules
}
