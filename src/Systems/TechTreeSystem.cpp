#include "Systems/TechTreeSystem.h"
#include "Systems/CraftingSystem.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>

TechTreeSystem::TechTreeSystem(SDLManager& sdlManager, DataManagement::GameDataManager* dataMgr, 
                               CraftingSystem* craftingSys)
    : sdlManager(sdlManager), dataManager(dataMgr), craftingSystem(craftingSys) {
    techTree = std::make_unique<TechTree>();
}

bool TechTreeSystem::initialize() {
    if (!techTree) {
        return false;
    }
    
    // Try to load from JSON file first
    bool loadedFromJson = loadTechTreeFromJson("data/tech_tree.json");
    if (!loadedFromJson) {
        std::cout << "Failed to load tech tree from JSON, falling back to hardcoded initialization" << std::endl;
        // Initialize basic technologies as fallback
        initializeBasicTechs();
    }
    
    // Set callback functions
    techTree->setOnTechCompleted([this](const std::string& techId) {
        handleTechCompletion(techId);
    });
    
    techTree->setOnResourceCheck([this](const std::string& techId, int cost) {
        return checkResourceAvailability(cost);
    });
    
    // Validate tech tree integrity
    if (!techTree->validateTechTree()) {
        std::cerr << "Tech tree validation failed!" << std::endl;
        return false;
    }
    
    return true;
}

std::shared_ptr<TechTreeUI> TechTreeSystem::createUI(int x, int y, int width, int height) {
    if (!techTree) {
        return nullptr;
    }
    
    techTreeUI = std::make_unique<TechTreeUI>(x, y, width, height, sdlManager, *techTree);
    
    // Set UI callbacks
    techTreeUI->onTechSelected = [this](const std::string& techId) {
        std::cout << "Selected tech: " << techId << std::endl;
    };
    
    techTreeUI->onStartResearch = [this](const std::string& techId) -> bool {
        return startResearch(techId);
    };
    
    return std::shared_ptr<TechTreeUI>(techTreeUI.get(), [](TechTreeUI*) {
        // Custom deleter, doesn't actually delete object, because it's managed by unique_ptr
    });
}

void TechTreeSystem::update(float deltaTime) {
    if (currentResearchTech.empty()) {
        return;
    }
    
    auto tech = techTree->getTech(currentResearchTech);
    if (!tech || tech->status != TechStatus::RESEARCHING) {
        currentResearchTech.clear();
        return;
    }
    
    // Accumulate research time
    researchTimer += deltaTime;
    
    // Update research progress once per second
    if (researchTimer >= 1.0f) {
        int progressToAdd = researchRate;
        int newProgress = tech->currentProgress + progressToAdd;
        
        // Update research progress
        if (techTree->updateResearchProgress(currentResearchTech, newProgress)) {
            // Research completed
            currentResearchTech.clear();
        }
        
        // Update UI display
        if (techTreeUI) {
            techTreeUI->updateTechDisplay(tech->id);
        }
        
        researchTimer = 0.0f;
    }
}

bool TechTreeSystem::startResearch(const std::string& techId) {
    if (!techTree) {
        return false;
    }
    
    auto tech = techTree->getTech(techId);
    if (!tech) {
        return false;
    }
    
    // Check if already researching another tech
    if (!currentResearchTech.empty()) {
        std::cout << "Already researching another tech: " << currentResearchTech << std::endl;
        return false;
    }
    
    // Check if resources are sufficient
    if (!checkResourceAvailability(tech->researchCost)) {
        std::cout << "Insufficient resources for tech: " << techId << std::endl;
        return false;
    }
    
    // Try to start research
    if (techTree->startResearch(techId)) {
        currentResearchTech = techId;
        
        // Consume resources
        if (onResourceConsume) {
            onResourceConsume(tech->researchCost);
        }
        
        std::cout << "Started researching: " << tech->name << std::endl;
        return true;
    }
    
    return false;
}

void TechTreeSystem::stopResearch() {
    if (!currentResearchTech.empty()) {
        auto tech = techTree->getTech(currentResearchTech);
        if (tech) {
            // Change status back to available
            tech->status = TechStatus::AVAILABLE;
            std::cout << "Stopped researching: " << tech->name << std::endl;
        }
        currentResearchTech.clear();
    }
}

bool TechTreeSystem::isTechCompleted(const std::string& techId) const {
    auto tech = techTree->getTech(techId);
    return tech && tech->status == TechStatus::COMPLETED;
}

bool TechTreeSystem::isTechAvailable(const std::string& techId) const {
    auto tech = techTree->getTech(techId);
    return tech && tech->status == TechStatus::AVAILABLE;
}

void TechTreeSystem::initializeBasicTechs() {
    // Create basic survival technologies
    auto basicSurvival = std::make_shared<TechNode>(
        "basic_survival", "Basic Survival", 
        "Learn basic survival skills, including gathering and simple tool crafting.",
        TechType::SURVIVAL, 50, 100, 100
    );
    basicSurvival->addReward("recipe", "medkit");
    basicSurvival->status = TechStatus::AVAILABLE; // Starting technology
    techTree->addTech(basicSurvival);
    
    // Basic crafting
    auto basicCrafting = std::make_shared<TechNode>(
        "basic_crafting", "Basic Crafting", 
        "Master basic crafting techniques, able to create more complex tools.",
        TechType::CRAFTING, 100, 300, 100
    );
    basicCrafting->addReward("recipe", "fuel");
    techTree->addTech(basicCrafting);
    techTree->setPrerequisite("basic_crafting", "basic_survival");
    
    // Basic farming
    auto basicFarming = std::make_shared<TechNode>(
        "basic_farming", "Basic Agriculture", 
        "Learn to grow crops, providing a stable food source.",
        TechType::AGRICULTURE, 150, 100, 250
    );
    basicFarming->addReward("recipe", "nutrition_meal");
    basicFarming->addReward("building", "farm_plot");
    techTree->addTech(basicFarming);
    techTree->setPrerequisite("basic_farming", "basic_survival");
    
    // Basic building
    auto basicBuilding = std::make_shared<TechNode>(
        "basic_building", "Basic Construction", 
        "Build simple structures to provide protection and storage space.",
        TechType::BUILDING, 200, 300, 250
    );
    basicBuilding->addReward("building", "wooden_wall");
    basicBuilding->addReward("building", "storage_box");
    techTree->addTech(basicBuilding);
    techTree->setPrerequisite("basic_building", "basic_crafting");
    
    // Advanced crafting
    auto advancedCrafting = std::make_shared<TechNode>(
        "advanced_crafting", "Advanced Crafting", 
        "Master metalworking and advanced tool crafting techniques.",
        TechType::CRAFTING, 300, 500, 100
    );
    advancedCrafting->addReward("recipe", "enhanced_weapon");
    advancedCrafting->addReward("recipe", "toolbox");
    techTree->addTech(advancedCrafting);
    techTree->setPrerequisite("advanced_crafting", "basic_crafting");
    
    // Military technology
    auto militaryTech = std::make_shared<TechNode>(
        "military_tech", "Military Technology", 
        "Develop weapons and defense technology to protect yourself.",
        TechType::MILITARY, 250, 500, 250
    );
    militaryTech->addReward("recipe", "wall");
    militaryTech->addReward("building", "watchtower");
    techTree->addTech(militaryTech);
    techTree->setPrerequisite("military_tech", "advanced_crafting");
    techTree->setPrerequisite("military_tech", "basic_building");
    
    // Advanced agriculture
    auto advancedFarming = std::make_shared<TechNode>(
        "advanced_farming", "Advanced Agriculture", 
        "Develop irrigation and high-yield agricultural techniques.",
        TechType::AGRICULTURE, 350, 100, 400
    );
    advancedFarming->addReward("building", "irrigation_system");
    techTree->addTech(advancedFarming);
    techTree->setPrerequisite("advanced_farming", "basic_farming");
    
    // Technology research
    auto techResearch = std::make_shared<TechNode>(
        "tech_research", "Technology Research", 
        "Establish research facilities to accelerate technological development.",
        TechType::ADVANCED, 500, 700, 150
    );
    techResearch->addReward("building", "research_lab");
    techResearch->addReward("passive", "research_speed_boost");
    techTree->addTech(techResearch);
    techTree->setPrerequisite("tech_research", "advanced_crafting");
    techTree->setPrerequisite("tech_research", "basic_building");
}

void TechTreeSystem::handleTechCompletion(const std::string& techId) {
    auto tech = techTree->getTech(techId);
    if (!tech) {
        return;
    }
    
    std::cout << "Tech completed: " << tech->name << std::endl;
    
    // Apply rewards (including recipe unlocks)
    applyTechRewards(tech->rewards);
    
    // Unlock related recipes based on tech ID
    unlockTechRelatedRecipes(techId);
    
    // Trigger system callback
    if (onTechCompleted) {
        onTechCompleted(techId, tech->rewards);
    }
    
    // Update UI display
    if (techTreeUI) {
        techTreeUI->updateTechDisplay(techId);
    }
}

void TechTreeSystem::applyTechRewards(const std::vector<TechReward>& rewards) {
    for (const auto& reward : rewards) {
        std::cout << "Applying reward: " << reward.type << " - " << reward.identifier;
        if (reward.amount > 1) {
            std::cout << " x" << reward.amount;
        }
        std::cout << std::endl;
        
        // Apply rewards based on reward type
        if (reward.type == "recipe") {
            // Unlock new recipe through crafting system
            if (craftingSystem) {
                craftingSystem->unlockRecipe(reward.identifier);
            }
        } else if (reward.type == "building") {
            // Unlock new building
            // dataManager->unlockBuilding(reward.identifier);
        } else if (reward.type == "passive") {
            // Apply passive effects
            if (reward.identifier == "research_speed_boost") {
                researchRate += 1; // Increase research speed
            }
        }
    }
}

bool TechTreeSystem::checkResourceAvailability(int cost) const {
    return researchPoints >= cost;
}

bool TechTreeSystem::saveTechTreeState(const std::string& filename) const {
    if (!techTree) {
        return false;
    }
    
    try {
        std::string jsonData = techTree->serializeToJson();
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonData;
        file.close();
        
        std::cout << "Tech tree state saved to: " << filename << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to save tech tree state: " << e.what() << std::endl;
        return false;
    }
}

bool TechTreeSystem::loadTechTreeState(const std::string& filename) {
    if (!techTree) {
        return false;
    }
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string jsonData((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        bool success = techTree->deserializeFromJson(jsonData);
        if (success) {
            // Refresh UI display
            if (techTreeUI) {
                techTreeUI->refreshTechButtons();
            }
            std::cout << "Tech tree state loaded from: " << filename << std::endl;
        }
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load tech tree state: " << e.what() << std::endl;
        return false;
    }
}

void TechTreeSystem::resetTechTree() {
    if (techTree) {
        techTree->resetAllTechs();
        currentResearchTech.clear();
        researchPoints = 0;
        researchRate = 1;
        researchTimer = 0.0f;
        
        // Re-initialize basic technologies
        initializeBasicTechs();
        
        // Refresh UI
        if (techTreeUI) {
            techTreeUI->refreshTechButtons();
        }
        
        std::cout << "Tech tree reset to initial state" << std::endl;
    }
}

void TechTreeSystem::testTriggerTechCompletion(const std::string& techId) {
    handleTechCompletion(techId);
}

bool TechTreeSystem::loadTechTreeFromJson(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open tech tree file: " << filename << std::endl;
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        file.close();
        
        return loadTechTreeFromJsonData(jsonData);
    } catch (const std::exception& e) {
        std::cerr << "Error loading tech tree from JSON: " << e.what() << std::endl;
        return false;
    }
}

bool TechTreeSystem::loadTechTreeFromJsonData(const nlohmann::json& jsonData) {
    try {
        if (!jsonData.contains("tech_tree") || !jsonData["tech_tree"].contains("technologies")) {
            std::cerr << "Invalid tech tree JSON structure" << std::endl;
            return false;
        }
        
        const auto& techTree_json = jsonData["tech_tree"];
        const auto& technologies = techTree_json["technologies"];
        
        if (!technologies.is_array()) {
            std::cerr << "Technologies field must be an array" << std::endl;
            return false;
        }
        
        // Clear existing tech tree
        techTree = std::make_unique<TechTree>();
        
        // First pass: Create all technologies
        for (const auto& tech_json : technologies) {
            if (!tech_json.contains("id") || !tech_json.contains("name") || 
                !tech_json.contains("description") || !tech_json.contains("type") ||
                !tech_json.contains("research_cost") || !tech_json.contains("position")) {
                std::cerr << "Technology missing required fields" << std::endl;
                continue;
            }
            
            std::string id = tech_json["id"];
            std::string name = tech_json["name"];
            std::string description = tech_json["description"];
            std::string type_str = tech_json["type"];
            int research_cost = tech_json["research_cost"];
            
            // Parse position
            const auto& position = tech_json["position"];
            int x = position.contains("x") ? position["x"].get<int>() : 0;
            int y = position.contains("y") ? position["y"].get<int>() : 0;
            
            // Convert type string to enum
            TechType type = TechType::SURVIVAL;
            if (type_str == "CRAFTING") type = TechType::CRAFTING;
            else if (type_str == "AGRICULTURE") type = TechType::AGRICULTURE;
            else if (type_str == "BUILDING") type = TechType::BUILDING;
            else if (type_str == "MILITARY") type = TechType::MILITARY;
            else if (type_str == "ADVANCED") type = TechType::ADVANCED;
            
            // Create technology node
            auto techNode = std::make_shared<TechNode>(id, name, description, type, research_cost, x, y);
            
            // Set initial status
            if (tech_json.contains("initial_status")) {
                std::string status_str = tech_json["initial_status"];
                if (status_str == "AVAILABLE") {
                    techNode->status = TechStatus::AVAILABLE;
                } else if (status_str == "RESEARCHING") {
                    techNode->status = TechStatus::RESEARCHING;
                } else if (status_str == "COMPLETED") {
                    techNode->status = TechStatus::COMPLETED;
                } else {
                    techNode->status = TechStatus::LOCKED;
                }
            }
            
            // Add rewards
            if (tech_json.contains("rewards") && tech_json["rewards"].is_array()) {
                for (const auto& reward_json : tech_json["rewards"]) {
                    if (reward_json.contains("type") && reward_json.contains("identifier")) {
                        std::string reward_type = reward_json["type"];
                        std::string identifier = reward_json["identifier"];
                        int amount = reward_json.contains("amount") ? reward_json["amount"].get<int>() : 1;
                        
                        techNode->addReward(reward_type, identifier, amount);
                    }
                }
            }
            
            // Add to tech tree
            techTree->addTech(techNode);
        }
        
        // Second pass: Set up prerequisites and unlocks
        for (const auto& tech_json : technologies) {
            std::string tech_id = tech_json["id"];
            
            // Set prerequisites
            if (tech_json.contains("prerequisites") && tech_json["prerequisites"].is_array()) {
                for (const auto& prereq_json : tech_json["prerequisites"]) {
                    if (prereq_json.contains("tech_id")) {
                        std::string prereq_tech_id = prereq_json["tech_id"];
                        bool required = prereq_json.contains("required") ? prereq_json["required"].get<bool>() : true;
                        
                        techTree->setPrerequisite(tech_id, prereq_tech_id, required);
                    }
                }
            }
        }
        
        std::cout << "Successfully loaded tech tree from JSON with " << technologies.size() << " technologies" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error parsing tech tree JSON data: " << e.what() << std::endl;
        return false;
    }
}

void TechTreeSystem::unlockTechRelatedRecipes(const std::string& techId) {
    if (!craftingSystem) {
        return;
    }
    
    // Define tech-to-recipe mappings
    // This maps completed technologies to recipes that should be unlocked
    if (techId == "basic_survival") {
        craftingSystem->unlockRecipe("basic_tools");
        craftingSystem->unlockRecipe("simple_shelter");
        std::cout << "Unlocked basic survival recipes" << std::endl;
        
    } else if (techId == "basic_crafting") {
        craftingSystem->unlockRecipe("advanced_tools");
        craftingSystem->unlockRecipe("wooden_weapons");
        std::cout << "Unlocked basic crafting recipes" << std::endl;
        
    } else if (techId == "basic_farming") {
        craftingSystem->unlockRecipe("farming_tools");
        craftingSystem->unlockRecipe("food_processing");
        std::cout << "Unlocked farming recipes" << std::endl;
        
    } else if (techId == "basic_building") {
        craftingSystem->unlockRecipe("building_materials");
        craftingSystem->unlockRecipe("construction_tools");
        std::cout << "Unlocked construction recipes" << std::endl;
        
    } else if (techId == "advanced_crafting") {
        craftingSystem->unlockRecipe("metal_tools");
        craftingSystem->unlockRecipe("advanced_weapons");
        std::cout << "Unlocked advanced crafting recipes" << std::endl;
        
    } else if (techId == "advanced_farming") {
        craftingSystem->unlockRecipe("irrigation_systems");
        craftingSystem->unlockRecipe("crop_enhancement");
        std::cout << "Unlocked advanced farming recipes" << std::endl;
        
    } else if (techId == "military_tech") {
        craftingSystem->unlockRecipe("combat_gear");
        craftingSystem->unlockRecipe("defensive_structures");
        std::cout << "Unlocked military technology recipes" << std::endl;
        
    } else if (techId == "tech_research") {
        craftingSystem->unlockRecipe("research_equipment");
        craftingSystem->unlockRecipe("knowledge_preservation");
        std::cout << "Unlocked research technology recipes" << std::endl;
    }
}
