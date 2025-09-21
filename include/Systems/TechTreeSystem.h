#pragma once
#include "Interface/ui/TechTree.h"
#include "Interface/ui/TechTreeUI.h"
#include "Systems/DataManager.h"
#include <memory>
#include <functional>

/**
 * @brief Tech tree system
 * High-level manager integrating tech tree logic, UI and game systems
 */
class TechTreeSystem {
private:
    std::unique_ptr<TechTree> techTree;                                 ///< Tech tree logic
    std::unique_ptr<TechTreeUI> techTreeUI;                             ///< Tech tree UI
    DataManagement::GameDataManager* dataManager;                     ///< Data manager
    
    // Research related
    std::string currentResearchTech;                                    ///< Currently researched technology
    int researchPoints = 0;                                             ///< Research points
    int researchRate = 1;                                               ///< Research speed per second
    
    // Callback functions
    std::function<void(const std::string&, const std::vector<TechReward>&)> onTechCompleted;
    std::function<bool(int)> onResourceConsume;                         ///< Resource consumption callback
    
    // Timer
    float researchTimer = 0.0f;                                         ///< Research timer

public:
    /**
     * @brief Constructor
     * @param sdlManager SDL manager
     * @param dataManager Data manager
     */
    TechTreeSystem(SDLManager& sdlManager, DataManagement::GameDataManager* dataMgr);
    
    /**
     * @brief Destructor
     */
    ~TechTreeSystem() = default;
    
    /**
     * @brief Initialize tech tree system
     * @return Whether initialization was successful
     */
    bool initialize();
    
    /**
     * @brief Create UI component
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Width
     * @param height Height
     * @return UI component pointer
     */
    std::shared_ptr<TechTreeUI> createUI(int x, int y, int width, int height);
    
    /**
     * @brief Update system (process automatic research progress)
     * @param deltaTime Time difference (seconds)
     */
    void update(float deltaTime);
    
    /**
     * @brief Set research points
     * @param points Research points
     */
    void setResearchPoints(int points) { researchPoints = points; }
    
    /**
     * @brief Get research points
     * @return Current research points
     */
    int getResearchPoints() const { return researchPoints; }
    
    /**
     * @brief Add research points
     * @param points Points to add
     */
    void addResearchPoints(int points) { researchPoints += points; }
    
    /**
     * @brief Set research rate
     * @param rate Research points per second
     */
    void setResearchRate(int rate) { researchRate = rate; }
    
    /**
     * @brief Start researching technology
     * @param techId Technology ID
     * @return Whether start was successful
     */
    bool startResearch(const std::string& techId);
    
    /**
     * @brief Stop current research
     */
    void stopResearch();
    
    /**
     * @brief Get currently researched technology
     * @return Technology ID, empty string if none
     */
    std::string getCurrentResearch() const { return currentResearchTech; }
    
    /**
     * @brief Check if technology is completed
     * @param techId Technology ID
     * @return Whether completed
     */
    bool isTechCompleted(const std::string& techId) const;
    
    /**
     * @brief Check if technology is available
     * @param techId Technology ID
     * @return Whether available
     */
    bool isTechAvailable(const std::string& techId) const;
    
    /**
     * @brief Get tech tree manager
     * @return Tech tree pointer
     */
    TechTree* getTechTree() const { return techTree.get(); }
    
    /**
     * @brief Set technology completion callback
     * @param callback Callback function
     */
    void setOnTechCompleted(std::function<void(const std::string&, const std::vector<TechReward>&)> callback) {
        onTechCompleted = callback;
    }
    
    /**
     * @brief Set resource consumption callback
     * @param callback Callback function
     */
    void setOnResourceConsume(std::function<bool(int)> callback) {
        onResourceConsume = callback;
    }
    
    /**
     * @brief Save tech tree state
     * @param filename File name
     * @return Whether save was successful
     */
    bool saveTechTreeState(const std::string& filename) const;
    
    /**
     * @brief Load tech tree state
     * @param filename File name
     * @return Whether load was successful
     */
    bool loadTechTreeState(const std::string& filename);
    
    /**
     * @brief Reset tech tree to initial state
     */
    void resetTechTree();

private:
    /**
     * @brief Initialize basic tech tree
     */
    void initializeBasicTechs();
    
    /**
     * @brief Handle technology completion
     * @param techId ID of completed technology
     */
    void handleTechCompletion(const std::string& techId);
    
    /**
     * @brief Apply technology rewards
     * @param rewards Reward list
     */
    void applyTechRewards(const std::vector<TechReward>& rewards);
    
    /**
     * @brief Check if resources are sufficient
     * @param cost Required resources
     * @return Whether sufficient
     */
    bool checkResourceAvailability(int cost) const;
    
    SDLManager& sdlManager; ///< SDL manager reference
};