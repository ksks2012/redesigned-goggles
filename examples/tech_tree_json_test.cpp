#include <iostream>
#include "Systems/TechTreeSystem.h"
#include "Systems/GameDataValidator.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <set>

int main() {
    std::cout << "=== Simplified Tech Tree JSON Loading Test ===" << std::endl;
    
    // 1. Validate data
    std::cout << "\n1. Validating game data..." << std::endl;
    GameDataValidator validator;
    if (!validator.loadDataFiles("data/materials.json", "data/recipes.json", "data/tech_tree.json")) {
        std::cerr << "❌ Unable to load data files" << std::endl;
        return 1;
    }
    
    auto validationResult = validator.validateAll();
    if (!validationResult.isValid) {
        std::cerr << "❌ Data validation failed:" << std::endl;
        std::cout << validator.getValidationReport(validationResult) << std::endl;
        return 1;
    }
    
    std::cout << "✅ Data validation passed" << std::endl;
    
    // 2. Test JSON parsing directly
    std::cout << "\n2. Testing JSON parsing..." << std::endl;
    try {
        std::ifstream file("data/tech_tree.json");
        if (!file.is_open()) {
            std::cerr << "❌ Unable to open tech_tree.json" << std::endl;
            return 1;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        file.close();
        
        std::cout << "✅ JSON file parsed successfully" << std::endl;
        
        // Check JSON structure
        if (jsonData.contains("tech_tree") && jsonData["tech_tree"].contains("technologies")) {
            auto technologies = jsonData["tech_tree"]["technologies"];
            std::cout << "✅ Found tech tree data with " << technologies.size() << " technologies" << std::endl;
            
            // Display basic information for each technology
            std::cout << "\n3. Loaded technologies:" << std::endl;
            for (const auto& tech : technologies) {
                if (tech.contains("id") && tech.contains("name")) {
                    std::cout << "  - " << tech["name"].get<std::string>() 
                             << " (" << tech["id"].get<std::string>() << ")" << std::endl;
                    
                    if (tech.contains("description")) {
                        std::cout << "    Description: " << tech["description"].get<std::string>() << std::endl;
                    }
                    
                    if (tech.contains("research_cost")) {
                        std::cout << "    Research cost: " << tech["research_cost"].get<int>() << std::endl;
                    }
                    
                    if (tech.contains("initial_status")) {
                        std::cout << "    Initial status: " << tech["initial_status"].get<std::string>() << std::endl;
                    }
                    
                    // Show prerequisites
                    if (tech.contains("prerequisites") && tech["prerequisites"].is_array()) {
                        auto prereqs = tech["prerequisites"];
                        if (!prereqs.empty()) {
                            std::cout << "    Prerequisites: ";
                            for (size_t i = 0; i < prereqs.size(); ++i) {
                                if (prereqs[i].contains("tech_id")) {
                                    std::cout << prereqs[i]["tech_id"].get<std::string>();
                                    if (i < prereqs.size() - 1) std::cout << ", ";
                                }
                            }
                            std::cout << std::endl;
                        }
                    }
                    
                    // Show rewards
                    if (tech.contains("rewards") && tech["rewards"].is_array()) {
                        auto rewards = tech["rewards"];
                        if (!rewards.empty()) {
                            std::cout << "    Rewards: ";
                            for (size_t i = 0; i < rewards.size(); ++i) {
                                if (rewards[i].contains("type") && rewards[i].contains("identifier")) {
                                    std::cout << rewards[i]["type"].get<std::string>() 
                                             << ":" << rewards[i]["identifier"].get<std::string>();
                                    if (i < rewards.size() - 1) std::cout << ", ";
                                }
                            }
                            std::cout << std::endl;
                        }
                    }
                    
                    std::cout << std::endl;
                }
            }
            
            std::cout << "\n4. Validating tech tree structure..." << std::endl;
            
            // Check dependencies
            std::set<std::string> techIds;
            for (const auto& tech : technologies) {
                if (tech.contains("id")) {
                    techIds.insert(tech["id"].get<std::string>());
                }
            }
            
            bool dependenciesValid = true;
            for (const auto& tech : technologies) {
                if (tech.contains("id") && tech.contains("prerequisites")) {
                    std::string techId = tech["id"].get<std::string>();
                    auto prereqs = tech["prerequisites"];
                    
                    for (const auto& prereq : prereqs) {
                        if (prereq.contains("tech_id")) {
                            std::string prereqId = prereq["tech_id"].get<std::string>();
                            if (techIds.find(prereqId) == techIds.end()) {
                                std::cout << "❌ Technology " << techId << " has non-existent prerequisite " 
                                         << prereqId << std::endl;
                                dependenciesValid = false;
                            }
                        }
                    }
                }
            }
            
            if (dependenciesValid) {
                std::cout << "✅ All dependencies are valid" << std::endl;
            }
            
            std::cout << "\n✅ JSON configuration system test completed!" << std::endl;
            
        } else {
            std::cerr << "❌ Invalid JSON structure" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ JSON parsing error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
