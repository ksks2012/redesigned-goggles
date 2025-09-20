#include "Interface/ui/TechNode.h"
#include <algorithm>

TechNode::TechNode(const std::string& techId, const std::string& techName, 
                   const std::string& desc, TechType techType, int cost, 
                   int posX, int posY)
    : id(techId), name(techName), description(desc), type(techType),
      researchCost(cost), x(posX), y(posY) {
    // Initialize with locked status
    status = TechStatus::LOCKED;
    currentProgress = 0;
}

void TechNode::addPrerequisite(const std::string& prereqId, bool required) {
    prerequisites.emplace_back(prereqId, required);
}

void TechNode::addReward(const std::string& rewardType, const std::string& identifier, int amount) {
    rewards.emplace_back(rewardType, identifier, amount);
}

bool TechNode::updateProgress(int progress) {
    if (status != TechStatus::RESEARCHING) {
        return false;
    }
    
    currentProgress = std::min(progress, researchCost);
    
    // Check if completed
    if (currentProgress >= researchCost) {
        complete();
        return true;
    }
    
    return false;
}

bool TechNode::canBeResearched() const {
    // Only available technologies can be researched
    if (status != TechStatus::AVAILABLE) {
        return false;
    }
    
    // Check custom conditions
    if (canStartResearch && !canStartResearch()) {
        return false;
    }
    
    return true;
}

void TechNode::complete() {
    status = TechStatus::COMPLETED;
    currentProgress = researchCost;
    
    // Trigger completion callback
    if (onCompleted) {
        onCompleted();
    }
}

void TechNode::reset() {
    status = TechStatus::LOCKED;
    currentProgress = 0;
}

float TechNode::getProgressPercent() const {
    if (researchCost == 0) {
        return status == TechStatus::COMPLETED ? 1.0f : 0.0f;
    }
    
    return static_cast<float>(currentProgress) / static_cast<float>(researchCost);
}

std::string TechNode::getStatusText() const {
    switch (status) {
        case TechStatus::LOCKED:
            return "Locked";
        case TechStatus::AVAILABLE:
            return "Researchable";
        case TechStatus::RESEARCHING:
            return "Researching (" + std::to_string(currentProgress) + "/" + 
                   std::to_string(researchCost) + ")";
        case TechStatus::COMPLETED:
            return "Completed";
        default:
            return "Unknown Status";
    }
}
