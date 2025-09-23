#include "Systems/TechTreeSystem.h"
#include "Data/DataManager.h"
#include "Core/SDLManager.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "🧪 Testing Tech Tree Progress Bars - Direct Test" << std::endl;
    
    try {
        // Initialize SDL Manager
        SDLManager sdlManager("Progress Bar Test", 800, 600);
        if (!sdlManager.initialize()) {
            throw std::runtime_error("Failed to initialize SDL");
        }
        
        DataManager dataManager;
        TechTreeSystem techTreeSystem(sdlManager, &dataManager);
        
        if (!techTreeSystem.initialize()) {
            throw std::runtime_error("Failed to initialize tech tree system");
        }
        
        // Set research points to 200
        techTreeSystem.setResearchPoints(200);
        std::cout << "Set research points to: " << techTreeSystem.getResearchPoints() << std::endl;
        
        // Create UI
        auto techTreeUI = techTreeSystem.createUI(50, 50, 700, 500);
        if (!techTreeUI) {
            throw std::runtime_error("Failed to create tech tree UI");
        }
        
        // Start research on basic survival
        std::cout << "Attempting to start research on basic_survival..." << std::endl;
        if (techTreeSystem.startResearch("basic_survival")) {
            std::cout << "✅ Research started successfully!" << std::endl;
            
            // Simulate some research progress
            for (int i = 0; i < 10; i++) {
                techTreeSystem.update(0.1f); // 100ms per update
                
                auto tech = techTreeSystem.getTechTree()->getTech("basic_survival");
                if (tech) {
                    float progress = tech->getProgressPercent() * 100.0f;
                    std::cout << "Research progress: " << progress << "%" << std::endl;
                    
                    if (tech->status == TechStatus::COMPLETED) {
                        std::cout << "🎉 Research completed!" << std::endl;
                        break;
                    }
                }
                
                // Small delay
                SDL_Delay(100);
            }
        } else {
            std::cout << "❌ Failed to start research" << std::endl;
        }
        
        std::cout << "✅ Test completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
