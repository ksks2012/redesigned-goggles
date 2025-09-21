#include <catch2/catch.hpp>
#include "Interface/ui/TechNode.h"
#include "Interface/ui/TechTree.h"
#include "Systems/TechTreeSystem.h"
#include <memory>

TEST_CASE("TechNode basic functionality", "[tech_tree]") {
    SECTION("TechNode creation and basic properties") {
        auto tech = std::make_shared<TechNode>(
            "test_tech", "Test Technology", 
            "A test technology for unit testing",
            TechType::SURVIVAL, 100, 50, 50
        );
        
        REQUIRE(tech->id == "test_tech");
        REQUIRE(tech->name == "Test Technology");
        REQUIRE(tech->description == "A test technology for unit testing");
        REQUIRE(tech->type == TechType::SURVIVAL);
        REQUIRE(tech->researchCost == 100);
        REQUIRE(tech->currentProgress == 0);
        REQUIRE(tech->status == TechStatus::LOCKED);
        REQUIRE(tech->x == 50);
        REQUIRE(tech->y == 50);
    }
    
    SECTION("TechNode progress updates") {
        auto tech = std::make_shared<TechNode>(
            "research_tech", "Research Tech", 
            "Tech for testing research progress",
            TechType::CRAFTING, 200, 0, 0
        );
        
        // Set to researching status
        tech->status = TechStatus::RESEARCHING;
        
        // Test progress updates
        REQUIRE_FALSE(tech->updateProgress(50));  // Not completed
        REQUIRE(tech->currentProgress == 50);
        REQUIRE(tech->getProgressPercent() == Approx(0.25f));
        
        REQUIRE_FALSE(tech->updateProgress(150)); // Still not completed
        REQUIRE(tech->currentProgress == 150);
        
        REQUIRE(tech->updateProgress(200));       // Completed
        REQUIRE(tech->status == TechStatus::COMPLETED);
        REQUIRE(tech->currentProgress == 200);
        REQUIRE(tech->getProgressPercent() == Approx(1.0f));
    }
    
    SECTION("TechNode prerequisites") {
        auto tech = std::make_shared<TechNode>(
            "advanced_tech", "Advanced Tech", 
            "Tech with prerequisites",
            TechType::ADVANCED, 300, 0, 0
        );
        
        tech->addPrerequisite("basic_tech_1", true);
        tech->addPrerequisite("basic_tech_2", false);  // Optional prerequisite
        
        REQUIRE(tech->prerequisites.size() == 2);
        REQUIRE(tech->prerequisites[0].techId == "basic_tech_1");
        REQUIRE(tech->prerequisites[0].required == true);
        REQUIRE(tech->prerequisites[1].techId == "basic_tech_2");
        REQUIRE(tech->prerequisites[1].required == false);
    }
    
    SECTION("TechNode rewards") {
        auto tech = std::make_shared<TechNode>(
            "reward_tech", "Reward Tech", 
            "Tech with rewards",
            TechType::BUILDING, 150, 0, 0
        );
        
        tech->addReward("recipe", "advanced_tool", 1);
        tech->addReward("building", "workshop", 1);
        tech->addReward("resource", "research_points", 50);
        
        REQUIRE(tech->rewards.size() == 3);
        REQUIRE(tech->rewards[0].type == "recipe");
        REQUIRE(tech->rewards[0].identifier == "advanced_tool");
        REQUIRE(tech->rewards[1].type == "building");
        REQUIRE(tech->rewards[2].amount == 50);
    }
}

TEST_CASE("TechTree functionality", "[tech_tree]") {
    TechTree techTree;
    
    SECTION("Adding and retrieving techs") {
        auto tech1 = std::make_shared<TechNode>("tech1", "Tech 1", "First tech", TechType::SURVIVAL, 50, 0, 0);
        auto tech2 = std::make_shared<TechNode>("tech2", "Tech 2", "Second tech", TechType::CRAFTING, 100, 100, 0);
        
        REQUIRE(techTree.addTech(tech1));
        REQUIRE(techTree.addTech(tech2));
        
        // Cannot add technologies with duplicate IDs
        auto tech1_duplicate = std::make_shared<TechNode>("tech1", "Duplicate", "Duplicate tech", TechType::SURVIVAL, 25, 0, 0);
        REQUIRE_FALSE(techTree.addTech(tech1_duplicate));
        
        // Test retrieval
        auto retrieved1 = techTree.getTech("tech1");
        auto retrieved2 = techTree.getTech("tech2");
        auto retrieved_null = techTree.getTech("nonexistent");
        
        REQUIRE(retrieved1 != nullptr);
        REQUIRE(retrieved1->name == "Tech 1");
        REQUIRE(retrieved2 != nullptr);
        REQUIRE(retrieved2->name == "Tech 2");
        REQUIRE(retrieved_null == nullptr);
    }
    
    SECTION("Prerequisites and dependencies") {
        auto basicTech = std::make_shared<TechNode>("basic", "Basic Tech", "Basic", TechType::SURVIVAL, 50, 0, 0);
        auto advancedTech = std::make_shared<TechNode>("advanced", "Advanced Tech", "Advanced", TechType::CRAFTING, 100, 100, 0);
        
        basicTech->status = TechStatus::COMPLETED; // Simulate completion
        
        techTree.addTech(basicTech);
        techTree.addTech(advancedTech);
        
        REQUIRE(techTree.setPrerequisite("advanced", "basic", true));
        
        // Check prerequisites
        REQUIRE(techTree.checkPrerequisites("advanced")); // basic completed, should be satisfied
        
        // Test unlock
        REQUIRE(techTree.unlockTech("advanced"));
        REQUIRE(advancedTech->status == TechStatus::AVAILABLE);
    }
    
    SECTION("Research workflow") {
        auto tech = std::make_shared<TechNode>("research_test", "Research Test", "Test research", TechType::SURVIVAL, 100, 0, 0);
        tech->status = TechStatus::AVAILABLE;
        
        techTree.addTech(tech);
        
        // Start research
        REQUIRE(techTree.startResearch("research_test"));
        REQUIRE(tech->status == TechStatus::RESEARCHING);
        
        // Update progress
        bool progressResult = techTree.updateResearchProgress("research_test", 50);
        REQUIRE_FALSE(progressResult);  // Should return false as tech is not completed yet
        REQUIRE(tech->currentProgress == 50);
        
        // Complete research
        REQUIRE(techTree.updateResearchProgress("research_test", 100));
        REQUIRE(tech->status == TechStatus::COMPLETED);
    }
    
    SECTION("Tech filtering by type and status") {
        auto survival1 = std::make_shared<TechNode>("s1", "Survival 1", "", TechType::SURVIVAL, 50, 0, 0);
        auto survival2 = std::make_shared<TechNode>("s2", "Survival 2", "", TechType::SURVIVAL, 75, 0, 50);
        auto crafting1 = std::make_shared<TechNode>("c1", "Crafting 1", "", TechType::CRAFTING, 100, 100, 0);
        
        survival1->status = TechStatus::COMPLETED;
        survival2->status = TechStatus::AVAILABLE;
        crafting1->status = TechStatus::RESEARCHING;
        
        techTree.addTech(survival1);
        techTree.addTech(survival2);
        techTree.addTech(crafting1);
        
        auto survivalTechs = techTree.getTechsByType(TechType::SURVIVAL);
        auto availableTechs = techTree.getAvailableTechs();
        auto completedTechs = techTree.getCompletedTechs();
        auto researchingTechs = techTree.getResearchingTechs();
        
        REQUIRE(survivalTechs.size() == 2);
        REQUIRE(availableTechs.size() == 1);
        REQUIRE(availableTechs[0]->id == "s2");
        REQUIRE(completedTechs.size() == 1);
        REQUIRE(completedTechs[0]->id == "s1");
        REQUIRE(researchingTechs.size() == 1);
        REQUIRE(researchingTechs[0]->id == "c1");
    }
}

TEST_CASE("TechTree validation", "[tech_tree]") {
    TechTree techTree;
    
    SECTION("Cyclic dependency detection") {
        auto tech1 = std::make_shared<TechNode>("tech1", "Tech 1", "", TechType::SURVIVAL, 50, 0, 0);
        auto tech2 = std::make_shared<TechNode>("tech2", "Tech 2", "", TechType::CRAFTING, 75, 100, 0);
        auto tech3 = std::make_shared<TechNode>("tech3", "Tech 3", "", TechType::BUILDING, 100, 0, 100);
        
        techTree.addTech(tech1);
        techTree.addTech(tech2);
        techTree.addTech(tech3);
        
        // Create cyclic dependency: tech1 -> tech2 -> tech3 -> tech1
        techTree.setPrerequisite("tech2", "tech1");
        techTree.setPrerequisite("tech3", "tech2");
        techTree.setPrerequisite("tech1", "tech3"); // This will create a cycle
        
        REQUIRE_FALSE(techTree.validateTechTree());
    }
    
    SECTION("Valid dependency chain") {
        auto tech1 = std::make_shared<TechNode>("tech1", "Tech 1", "", TechType::SURVIVAL, 50, 0, 0);
        auto tech2 = std::make_shared<TechNode>("tech2", "Tech 2", "", TechType::CRAFTING, 75, 100, 0);
        auto tech3 = std::make_shared<TechNode>("tech3", "Tech 3", "", TechType::BUILDING, 100, 0, 100);
        
        techTree.addTech(tech1);
        techTree.addTech(tech2);
        techTree.addTech(tech3);
        
        // Create valid linear dependency chain: tech1 -> tech2 -> tech3
        techTree.setPrerequisite("tech2", "tech1");
        techTree.setPrerequisite("tech3", "tech2");
        
        REQUIRE(techTree.validateTechTree());
    }
}

TEST_CASE("TechTree serialization", "[tech_tree]") {
    TechTree techTree;
    
    SECTION("Serialize and deserialize tech tree") {
        // Create test tech tree
        auto tech1 = std::make_shared<TechNode>("basic_survival", "Basic Survival", 
                                              "Basic survival skills", TechType::SURVIVAL, 50, 100, 100);
        tech1->status = TechStatus::COMPLETED;
        tech1->addReward("recipe", "stone_axe");
        
        auto tech2 = std::make_shared<TechNode>("basic_crafting", "Basic Crafting", 
                                              "Basic crafting skills", TechType::CRAFTING, 100, 300, 100);
        tech2->status = TechStatus::AVAILABLE;
        tech2->addReward("recipe", "wooden_spear");
        
        techTree.addTech(tech1);
        techTree.addTech(tech2);
        techTree.setPrerequisite("basic_crafting", "basic_survival");
        
        // Serialize
        std::string serialized = techTree.serializeToJson();
        REQUIRE_FALSE(serialized.empty());
        
        // Create new tech tree and deserialize
        TechTree newTechTree;
        REQUIRE(newTechTree.deserializeFromJson(serialized));
        
        // Verify deserialization results
        auto deserializedTech1 = newTechTree.getTech("basic_survival");
        auto deserializedTech2 = newTechTree.getTech("basic_crafting");
        
        REQUIRE(deserializedTech1 != nullptr);
        REQUIRE(deserializedTech1->name == "Basic Survival");
        REQUIRE(deserializedTech1->status == TechStatus::COMPLETED);
        REQUIRE(deserializedTech1->rewards.size() == 1);
        REQUIRE(deserializedTech1->rewards[0].identifier == "stone_axe");
        
        REQUIRE(deserializedTech2 != nullptr);
        REQUIRE(deserializedTech2->status == TechStatus::AVAILABLE);
        REQUIRE(deserializedTech2->prerequisites.size() == 1);
        REQUIRE(deserializedTech2->prerequisites[0].techId == "basic_survival");
    }
}
