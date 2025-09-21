#include "Interface/ui/TechTree.h"
#include "Systems/TechTreeSystem.h"
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

// Simplified SDL manager mock
class MockSDLManager {
public:
    void* getRenderer() { return nullptr; }
};

// Simplified data manager mock
class MockDataManager {
public:
    bool saveData(const std::string&, const nlohmann::json&) { return true; }
    nlohmann::json loadData(const std::string&) { return nlohmann::json::object(); }
};

int main() {
    std::cout << "=== Tech Tree Demo Program ===" << std::endl;
    
    try {
        // Create mock managers
        MockSDLManager mockSDL;
        MockDataManager mockData;
        
        // Create tech tree
        TechTree techTree;
        
        // Add some example technologies (using complete constructor parameters)
        auto basicSurvival = std::make_shared<TechNode>("basic_survival", "Basic Survival", "Learn basic survival skills", TechType::SURVIVAL, 100, 100, 50);
        basicSurvival->researchCost = 100;
        
        auto hunting = std::make_shared<TechNode>("hunting", "Hunting", "Learn hunting techniques", TechType::SURVIVAL, 250, 100, 50);
        hunting->researchCost = 150;
        hunting->prerequisites.push_back({"basic_survival", true});
        
        auto cooking = std::make_shared<TechNode>("cooking", "Cooking", "Learn cooking food", TechType::CRAFTING, 100, 200, 50);
        cooking->researchCost = 120;
        cooking->prerequisites.push_back({"basic_survival", true});
        
        auto advanced_hunting = std::make_shared<TechNode>("advanced_hunting", "Advanced Hunting", "Master advanced hunting techniques", TechType::SURVIVAL, 400, 100, 50);
        advanced_hunting->researchCost = 300;
        advanced_hunting->prerequisites.push_back({"hunting", true});
        advanced_hunting->prerequisites.push_back({"cooking", false}); // Optional prerequisite
        
        // Add technologies to tech tree
        techTree.addTech(basicSurvival);
        techTree.addTech(hunting);
        techTree.addTech(cooking);
        techTree.addTech(advanced_hunting);
        
        // Display tech tree status
        std::cout << "\n=== Initial Tech Tree Status ===" << std::endl;
        const auto& allTechs = techTree.getAllTechs();
        for (const auto& pair : allTechs) {
            const auto& tech = pair.second;
            std::cout << "- " << tech->name << " (" << tech->getStatusText() << ")" << std::endl;
        }
        
        // Simulate research workflow
        std::cout << "\n=== Start Researching Basic Survival ===" << std::endl;
        if (techTree.startResearch("basic_survival")) {
            std::cout << "Successfully started research: Basic Survival" << std::endl;
            
            // Simulate research progress
            for (int i = 0; i <= 5; i++) {
                techTree.updateResearchProgress("basic_survival", 20);
                auto tech = techTree.getTech("basic_survival");
                std::cout << "Research progress: " << (tech->getProgressPercent() * 100.0f) << "%" << std::endl;
            }
            
            // Check if completed
            auto basicSurvivalTech = techTree.getTech("basic_survival");
            if (basicSurvivalTech->status == TechStatus::COMPLETED) {
                std::cout << "Basic Survival research completed!" << std::endl;
            }
        }
        
        // Display unlocked technologies
        std::cout << "\n=== Updated Tech Tree Status ===" << std::endl;
        for (const auto& pair : allTechs) {
            const auto& tech = pair.second;
            std::cout << "- " << tech->name << " (" << tech->getStatusText() << ")" << std::endl;
        }
        
        // Test if hunting research can start
        std::cout << "\n=== Attempt to Research Hunting ===" << std::endl;
        if (techTree.startResearch("hunting")) {
            std::cout << "Can start researching hunting techniques!" << std::endl;
        } else {
            std::cout << "Cannot research hunting techniques yet" << std::endl;
        }
        
        // Test serialization
        std::cout << "\n=== Test Serialization ===" << std::endl;
        std::string saveData = techTree.serializeToJson();
        std::cout << "Tech tree data serialized" << std::endl;
        
        // Create new tech tree and deserialize
        TechTree newTechTree;
        newTechTree.addTech(std::make_shared<TechNode>("basic_survival", "Basic Survival", "Learn basic survival skills", TechType::SURVIVAL, 100, 100, 50));
        newTechTree.addTech(std::make_shared<TechNode>("hunting", "Hunting", "Learn hunting techniques", TechType::SURVIVAL, 250, 100, 50));
        newTechTree.addTech(std::make_shared<TechNode>("cooking", "Cooking", "Learn cooking food", TechType::CRAFTING, 100, 200, 50));
        newTechTree.addTech(std::make_shared<TechNode>("advanced_hunting", "Advanced Hunting", "Master advanced hunting techniques", TechType::SURVIVAL, 400, 100, 50));
        
        if (newTechTree.deserializeFromJson(saveData)) {
            std::cout << "Tech tree data deserialization successful!" << std::endl;
            
            // Verify status
            auto loadedTech = newTechTree.getTech("basic_survival");
            if (loadedTech && loadedTech->status == TechStatus::COMPLETED) {
                std::cout << "Loaded tech tree status is correct!" << std::endl;
            }
        }
        
        std::cout << "\n=== Demo Completed ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
