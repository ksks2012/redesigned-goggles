#include "Systems/TechTreeSystem.h"
#include "Systems/CraftingSystem.h"
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
    
    // Initialize basic technologies
    initializeBasicTechs();
    
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
