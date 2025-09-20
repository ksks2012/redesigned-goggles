#include "Interface/ui/TechTree.h"
#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool TechTree::addTech(std::shared_ptr<TechNode> tech) {
    if (!tech || tech->id.empty()) {
        return false;
    }
    
    // Check if tech with same ID already exists
    if (techNodes.find(tech->id) != techNodes.end()) {
        return false;
    }
    
    techNodes[tech->id] = tech;
    
    // Set tech completion callback
    tech->onCompleted = [this, techId = tech->id]() {
        updateDependentTechs(techId);
        if (onTechCompleted) {
            onTechCompleted(techId);
        }
    };
    
    return true;
}

std::shared_ptr<TechNode> TechTree::getTech(const std::string& techId) const {
    auto it = techNodes.find(techId);
    return it != techNodes.end() ? it->second : nullptr;
}

bool TechTree::setPrerequisite(const std::string& techId, const std::string& prerequisiteId, bool required) {
    auto tech = getTech(techId);
    auto prereqTech = getTech(prerequisiteId);
    
    if (!tech || !prereqTech) {
        return false;
    }
    
    // Add prerequisite to target tech
    tech->addPrerequisite(prerequisiteId, required);
    
    // Establish dependency relationship mapping
    dependencies[prerequisiteId].push_back(techId);
    
    return true;
}

bool TechTree::checkPrerequisites(const std::string& techId) const {
    auto tech = getTech(techId);
    if (!tech) {
        return false;
    }
    
    // Check all prerequisites
    for (const auto& prereq : tech->prerequisites) {
        auto prereqTech = getTech(prereq.techId);
        if (!prereqTech) {
            continue; // If prerequisite tech doesn't exist, skip
        }
        
        // Required conditions must be completed, optional conditions can be incomplete
        if (prereq.required && prereqTech->status != TechStatus::COMPLETED) {
            return false;
        }
    }
    
    return true;
}

bool TechTree::unlockTech(const std::string& techId) {
    auto tech = getTech(techId);
    if (!tech) {
        return false;
    }
    
    // Only locked technologies can be unlocked
    if (tech->status != TechStatus::LOCKED) {
        return false;
    }
    
    // Check prerequisites
    if (!checkPrerequisites(techId)) {
        return false;
    }
    
    tech->status = TechStatus::AVAILABLE;
    
    if (onTechUnlocked) {
        onTechUnlocked(techId);
    }
    
    return true;
}

bool TechTree::startResearch(const std::string& techId) {
    auto tech = getTech(techId);
    if (!tech) {
        return false;
    }
    
    // Check if research can be started
    if (!tech->canBeResearched()) {
        return false;
    }
    
    // Check if resources are sufficient
    if (onResourceCheck && !onResourceCheck(techId, tech->researchCost)) {
        return false;
    }
    
    tech->status = TechStatus::RESEARCHING;
    researchQueue.insert(techId);
    
    return true;
}

bool TechTree::updateResearchProgress(const std::string& techId, int progress) {
    auto tech = getTech(techId);
    if (!tech) {
        return false;
    }
    
    return tech->updateProgress(progress);
}

bool TechTree::completeTech(const std::string& techId) {
    auto tech = getTech(techId);
    if (!tech) {
        return false;
    }
    
    // Only researching technologies can be completed
    if (tech->status != TechStatus::RESEARCHING) {
        return false;
    }
    
    tech->complete();
    researchQueue.erase(techId);
    
    return true;
}

std::vector<std::shared_ptr<TechNode>> TechTree::getTechsByType(TechType type) const {
    std::vector<std::shared_ptr<TechNode>> result;
    
    for (const auto& pair : techNodes) {
        if (pair.second->type == type) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<TechNode>> TechTree::getAvailableTechs() const {
    std::vector<std::shared_ptr<TechNode>> result;
    
    for (const auto& pair : techNodes) {
        if (pair.second->status == TechStatus::AVAILABLE) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<TechNode>> TechTree::getCompletedTechs() const {
    std::vector<std::shared_ptr<TechNode>> result;
    
    for (const auto& pair : techNodes) {
        if (pair.second->status == TechStatus::COMPLETED) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<TechNode>> TechTree::getResearchingTechs() const {
    std::vector<std::shared_ptr<TechNode>> result;
    
    for (const auto& pair : techNodes) {
        if (pair.second->status == TechStatus::RESEARCHING) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

void TechTree::resetAllTechs() {
    for (auto& pair : techNodes) {
        pair.second->reset();
    }
    researchQueue.clear();
}

bool TechTree::validateTechTree() const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    
    // Check if each tech has cyclic dependencies
    for (const auto& pair : techNodes) {
        visited.clear();
        recursionStack.clear();
        
        if (hasCyclicDependency(pair.first, visited, recursionStack)) {
            std::cerr << "Cyclic dependency detected for tech: " << pair.first << std::endl;
            return false;
        }
    }
    
    return true;
}

std::string TechTree::serializeToJson() const {
    json techTreeJson;
    
    for (const auto& pair : techNodes) {
        const auto& tech = pair.second;
        json techJson;
        
        techJson["id"] = tech->id;
        techJson["name"] = tech->name;
        techJson["description"] = tech->description;
        techJson["type"] = static_cast<int>(tech->type);
        techJson["researchCost"] = tech->researchCost;
        techJson["currentProgress"] = tech->currentProgress;
        techJson["status"] = static_cast<int>(tech->status);
        techJson["x"] = tech->x;
        techJson["y"] = tech->y;
        techJson["width"] = tech->width;
        techJson["height"] = tech->height;
        techJson["iconPath"] = tech->iconPath;
        
        // Prerequisites
        json prereqsJson = json::array();
        for (const auto& prereq : tech->prerequisites) {
            json prereqJson;
            prereqJson["techId"] = prereq.techId;
            prereqJson["required"] = prereq.required;
            prereqsJson.push_back(prereqJson);
        }
        techJson["prerequisites"] = prereqsJson;
        
        // Rewards
        json rewardsJson = json::array();
        for (const auto& reward : tech->rewards) {
            json rewardJson;
            rewardJson["type"] = reward.type;
            rewardJson["identifier"] = reward.identifier;
            rewardJson["amount"] = reward.amount;
            rewardsJson.push_back(rewardJson);
        }
        techJson["rewards"] = rewardsJson;
        
        techTreeJson["techs"][tech->id] = techJson;
    }
    
    return techTreeJson.dump(2);
}

bool TechTree::deserializeFromJson(const std::string& jsonStr) {
    try {
        json techTreeJson = json::parse(jsonStr);
        
        // Clear existing data
        techNodes.clear();
        dependencies.clear();
        researchQueue.clear();
        
        // First load all tech nodes (without dependency relationships)
        for (const auto& item : techTreeJson["techs"].items()) {
            const json& techJson = item.value();
            
            auto tech = std::make_shared<TechNode>(
                techJson["id"],
                techJson["name"],
                techJson["description"],
                static_cast<TechType>(techJson["type"]),
                techJson["researchCost"],
                techJson["x"],
                techJson["y"]
            );
            
            tech->currentProgress = techJson["currentProgress"];
            tech->status = static_cast<TechStatus>(techJson["status"]);
            tech->width = techJson["width"];
            tech->height = techJson["height"];
            tech->iconPath = techJson["iconPath"];
            
            // Load rewards
            for (const auto& rewardJson : techJson["rewards"]) {
                tech->addReward(rewardJson["type"], rewardJson["identifier"], rewardJson["amount"]);
            }
            
            addTech(tech);
        }
        
        // Then set up dependency relationships
        for (const auto& item : techTreeJson["techs"].items()) {
            const json& techJson = item.value();
            std::string techId = techJson["id"];
            
            for (const auto& prereqJson : techJson["prerequisites"]) {
                setPrerequisite(techId, prereqJson["techId"], prereqJson["required"]);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to deserialize tech tree: " << e.what() << std::endl;
        return false;
    }
}

void TechTree::updateDependentTechs(const std::string& completedTechId) {
    auto it = dependencies.find(completedTechId);
    if (it == dependencies.end()) {
        return;
    }
    
    // Check all techs that depend on this technology
    for (const std::string& dependentTechId : it->second) {
        auto dependentTech = getTech(dependentTechId);
        if (!dependentTech || dependentTech->status != TechStatus::LOCKED) {
            continue;
        }
        
        // If prerequisites are satisfied, unlock the tech
        if (checkPrerequisites(dependentTechId)) {
            unlockTech(dependentTechId);
        }
    }
}

bool TechTree::hasCyclicDependency(const std::string& techId, 
                                  std::unordered_set<std::string>& visited,
                                  std::unordered_set<std::string>& recursionStack) const {
    visited.insert(techId);
    recursionStack.insert(techId);
    
    auto tech = getTech(techId);
    if (!tech) {
        return false;
    }
    
    // Check all prerequisites
    for (const auto& prereq : tech->prerequisites) {
        if (visited.find(prereq.techId) == visited.end()) {
            if (hasCyclicDependency(prereq.techId, visited, recursionStack)) {
                return true;
            }
        } else if (recursionStack.find(prereq.techId) != recursionStack.end()) {
            return true; // Found cyclic dependency
        }
    }
    
    recursionStack.erase(techId);
    return false;
}
