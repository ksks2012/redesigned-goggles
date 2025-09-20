#pragma once
#include "Interface/ui/TechNode.h"
#include <map>
#include <unordered_set>
#include <functional>

/**
 * @brief Tech tree manager
 * Responsible for managing all technology node logic and state
 */
class TechTree {
private:
    std::map<std::string, std::shared_ptr<TechNode>> techNodes;     ///< All technology nodes
    std::map<std::string, std::vector<std::string>> dependencies;   ///< Dependency relationship graph
    std::unordered_set<std::string> researchQueue;                 ///< Research queue
    
    // Callback functions
    std::function<void(const std::string&)> onTechCompleted;        ///< Technology completion callback
    std::function<void(const std::string&)> onTechUnlocked;         ///< Technology unlock callback
    std::function<bool(const std::string&, int)> onResourceCheck;   ///< Resource check callback
    
public:
    TechTree() = default;
    ~TechTree() = default;
    
    /**
     * @brief Add technology node
     * @param tech Technology node
     * @return Whether addition was successful
     */
    bool addTech(std::shared_ptr<TechNode> tech);
    
    /**
     * @brief Get technology node by ID
     * @param techId Technology ID
     * @return Technology node pointer, returns nullptr if not found
     */
    std::shared_ptr<TechNode> getTech(const std::string& techId) const;
    
    /**
     * @brief Set technology prerequisite
     * @param techId Target technology ID
     * @param prerequisiteId Prerequisite technology ID
     * @param required Whether it's a mandatory condition
     * @return Whether setting was successful
     */
    bool setPrerequisite(const std::string& techId, const std::string& prerequisiteId, bool required = true);
    
    /**
     * @brief Check if technology prerequisites are met
     * @param techId Technology ID
     * @return Whether prerequisites are met
     */
    bool checkPrerequisites(const std::string& techId) const;
    
    /**
     * @brief Unlock technology (make it researchable)
     * @param techId Technology ID
     * @return Whether unlock was successful
     */
    bool unlockTech(const std::string& techId);
    
    /**
     * @brief Start researching technology
     * @param techId Technology ID
     * @return Whether start was successful
     */
    bool startResearch(const std::string& techId);
    
    /**
     * @brief Update research progress
     * @param techId Technology ID
     * @param progress New progress value
     * @return Whether update was successful
     */
    bool updateResearchProgress(const std::string& techId, int progress);
    
    /**
     * @brief Complete technology research
     * @param techId Technology ID
     * @return Whether completion was successful
     */
    bool completeTech(const std::string& techId);
    
    /**
     * @brief Get all technology nodes
     * @return Constant reference to technology node map
     */
    const std::map<std::string, std::shared_ptr<TechNode>>& getAllTechs() const { return techNodes; }
    
    /**
     * @brief Get technologies of specified type
     * @param type Technology type
     * @return List of technology nodes of that type
     */
    std::vector<std::shared_ptr<TechNode>> getTechsByType(TechType type) const;
    
    /**
     * @brief Get list of researchable technologies
     * @return List of researchable technology nodes
     */
    std::vector<std::shared_ptr<TechNode>> getAvailableTechs() const;
    
    /**
     * @brief Get list of completed technologies
     * @return List of completed technology nodes
     */
    std::vector<std::shared_ptr<TechNode>> getCompletedTechs() const;
    
    /**
     * @brief Get list of technologies currently being researched
     * @return List of technology nodes being researched
     */
    std::vector<std::shared_ptr<TechNode>> getResearchingTechs() const;
    
    /**
     * @brief Reset all technology states
     */
    void resetAllTechs();
    
    /**
     * @brief Check integrity of tech tree
     * @return Whether it's complete (no circular dependencies, etc.)
     */
    bool validateTechTree() const;
    
    /**
     * @brief Set technology completion callback
     * @param callback Callback function
     */
    void setOnTechCompleted(std::function<void(const std::string&)> callback) {
        onTechCompleted = callback;
    }
    
    /**
     * @brief Set technology unlock callback
     * @param callback Callback function
     */
    void setOnTechUnlocked(std::function<void(const std::string&)> callback) {
        onTechUnlocked = callback;
    }
    
    /**
     * @brief Set resource check callback
     * @param callback Callback function
     */
    void setOnResourceCheck(std::function<bool(const std::string&, int)> callback) {
        onResourceCheck = callback;
    }
    
    /**
     * @brief Serialize tech tree state to JSON
     * @return JSON string
     */
    std::string serializeToJson() const;
    
    /**
     * @brief Deserialize tech tree state from JSON
     * @param jsonStr JSON string
     * @return Whether deserialization was successful
     */
    bool deserializeFromJson(const std::string& jsonStr);

private:
    /**
     * @brief Update dependent technology states
     * @param completedTechId ID of just completed technology
     */
    void updateDependentTechs(const std::string& completedTechId);
    
    /**
     * @brief Check for circular dependencies
     * @param techId Technology ID
     * @param visited Set of visited nodes
     * @param recursionStack Recursion stack
     * @return Whether circular dependency exists
     */
    bool hasCyclicDependency(const std::string& techId, 
                           std::unordered_set<std::string>& visited,
                           std::unordered_set<std::string>& recursionStack) const;
};