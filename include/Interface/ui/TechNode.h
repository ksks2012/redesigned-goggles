#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

/**
 * @brief Technology status enumeration
 */
enum class TechStatus {
    LOCKED,      ///< Not unlocked yet (prerequisites not met)
    AVAILABLE,   ///< Can be researched (prerequisites met)
    RESEARCHING, ///< Currently being researched
    COMPLETED    ///< Research completed
};

/**
 * @brief Technology research type
 */
enum class TechType {
    SURVIVAL,    ///< Survival technology
    CRAFTING,    ///< Crafting technology
    BUILDING,    ///< Building technology
    AGRICULTURE, ///< Agriculture technology
    MILITARY,    ///< Military technology
    ADVANCED     ///< Advanced technology
};

/**
 * @brief Technology prerequisite structure
 */
struct TechPrerequisite {
    std::string techId;     ///< Prerequisite technology ID
    bool required = true;   ///< Whether it's a mandatory condition
    
    TechPrerequisite(const std::string& id, bool req = true) 
        : techId(id), required(req) {}
};

/**
 * @brief Technology reward structure
 */
struct TechReward {
    std::string type;       ///< Reward type (recipe, building, etc.)
    std::string identifier; ///< Reward identifier
    int amount = 1;         ///< Quantity
    
    TechReward(const std::string& rewardType, const std::string& id, int amt = 1)
        : type(rewardType), identifier(id), amount(amt) {}
};

/**
 * @brief Technology node class
 * Represents a technology item in the tech tree
 */
class TechNode {
public:
    // Basic properties
    std::string id;                              ///< Unique identifier
    std::string name;                            ///< Display name
    std::string description;                     ///< Detailed description
    TechType type;                               ///< Technology type
    
    // Research related
    int researchCost = 0;                        ///< Required resources for research
    int currentProgress = 0;                     ///< Current research progress
    TechStatus status = TechStatus::LOCKED;      ///< Current status
    
    // Dependencies
    std::vector<TechPrerequisite> prerequisites; ///< Prerequisites
    std::vector<std::string> unlocks;           ///< Other technologies unlocked by this tech
    std::vector<TechReward> rewards;            ///< Rewards upon completion
    
    // UI related properties
    int x, y;                                   ///< UI position coordinates
    int width = 120, height = 80;              ///< UI dimensions
    std::string iconPath;                       ///< Icon file path
    
    // Callback functions
    std::function<void()> onCompleted;          ///< Callback when completed
    std::function<bool()> canStartResearch;     ///< Callback to check if research can start
    
public:
    /**
     * @brief Constructor
     * @param techId Technology unique identifier
     * @param techName Technology name
     * @param desc Technology description
     * @param techType Technology type
     * @param cost Research cost
     * @param posX UI X coordinate
     * @param posY UI Y coordinate
     */
    TechNode(const std::string& techId, const std::string& techName, 
             const std::string& desc, TechType techType, int cost, 
             int posX, int posY);
    
    /**
     * @brief Add prerequisite
     * @param prereqId Prerequisite technology ID
     * @param required Whether it's a mandatory condition
     */
    void addPrerequisite(const std::string& prereqId, bool required = true);
    
    /**
     * @brief Add reward
     * @param rewardType Reward type
     * @param identifier Reward identifier
     * @param amount Quantity
     */
    void addReward(const std::string& rewardType, const std::string& identifier, int amount = 1);
    
    /**
     * @brief Set icon
     * @param path Icon file path
     */
    void setIcon(const std::string& path) { iconPath = path; }
    
    /**
     * @brief Update research progress
     * @param progress New progress value
     * @return Whether research is completed
     */
    bool updateProgress(int progress);
    
    /**
     * @brief Check if research can be started
     * @return Whether research can be started
     */
    bool canBeResearched() const;
    
    /**
     * @brief Complete research
     */
    void complete();
    
    /**
     * @brief Reset technology state (for testing)
     */
    void reset();
    
    /**
     * @brief Get research progress percentage
     * @return Progress value from 0.0-1.0
     */
    float getProgressPercent() const;
    
    /**
     * @brief Get status description text
     * @return Status description
     */
    std::string getStatusText() const;
};