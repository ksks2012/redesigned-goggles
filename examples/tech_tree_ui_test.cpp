#include "Systems/TechTreeSystem.h"
#include "Systems/DataManager.h"
#include "Systems/SDLManager.h"
#include "Interface/ui/UIManager.h"
#include <iostream>

/**
 * @brief Tech Tree UI Label Test Program
 * Demonstrates the new UILabel-based TechNode rendering with connection lines
 */
class TechTreeUITest {
private:
    SDLManager sdlManager;
    DataManagement::GameDataManager dataManager;
    std::unique_ptr<TechTreeSystem> techTreeSystem;
    std::shared_ptr<UIManager> uiManager;
    std::shared_ptr<TechTreeUI> techTreeUI;
    
    bool running = true;
    
public:
    TechTreeUITest() {
        if (!sdlManager.initialize()) {
            throw std::runtime_error("Failed to initialize SDL");
        }
        
        uiManager = std::make_shared<UIManager>();
        initializeTechTreeSystem();
    }
    
    void initializeTechTreeSystem() {
        techTreeSystem = std::make_unique<TechTreeSystem>(sdlManager, &dataManager);
        
        if (!techTreeSystem->initialize()) {
            throw std::runtime_error("Failed to initialize tech tree system");
        }
        
        // Create a test tech tree with positioned nodes
        TechTree* techTree = techTreeSystem->getTechTree();
        
        // Create test technologies with specific positions for UILabel display
        auto basicSurvival = std::make_shared<TechNode>(
            "basic_survival", "Basic Survival", 
            "Essential survival skills", TechType::SURVIVAL, 50, 100, 150
        );
        basicSurvival->status = TechStatus::AVAILABLE;
        
        auto basicFarming = std::make_shared<TechNode>(
            "basic_farming", "Basic Agriculture", 
            "Learn to grow crops", TechType::AGRICULTURE, 150, 300, 100
        );
        basicFarming->addPrerequisite("basic_survival", true);
        
        auto basicBuilding = std::make_shared<TechNode>(
            "basic_building", "Basic Construction", 
            "Build simple structures", TechType::BUILDING, 200, 300, 200
        );
        basicBuilding->addPrerequisite("basic_survival", true);
        
        auto advancedFarming = std::make_shared<TechNode>(
            "advanced_farming", "Advanced Agriculture", 
            "Develop irrigation techniques", TechType::AGRICULTURE, 350, 500, 100
        );
        advancedFarming->addPrerequisite("basic_farming", true);
        
        // Add techs to the tree
        techTree->addTech(basicSurvival);
        techTree->addTech(basicFarming);
        techTree->addTech(basicBuilding);
        techTree->addTech(advancedFarming);
        
        // Create UI with larger size to show all nodes and connections
        techTreeUI = techTreeSystem->createUI(50, 50, 700, 500);
        if (techTreeUI) {
            uiManager->addComponent(techTreeUI);
            std::cout << "✅ Tech Tree UI with UILabel nodes created successfully!" << std::endl;
            std::cout << "   - UILabel components created for each TechNode" << std::endl;
            std::cout << "   - Connection lines will be drawn between prerequisites" << std::endl;
        }
        
        std::cout << "\n🎮 UILabel Tech Tree Test Controls:" << std::endl;
        std::cout << "   - Click on tech nodes (UILabels) to select them" << std::endl;
        std::cout << "   - Press 1-3 keys for quick selection" << std::endl;
        std::cout << "   - Press Enter to research selected tech" << std::endl;
        std::cout << "   - Press R to add research points" << std::endl;
        std::cout << "   - Press Q to quit" << std::endl;
    }
    
    void handleEvent(const SDL_Event& event) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_q:
                    running = false;
                    break;
                case SDLK_r:
                    techTreeSystem->setResearchPoints(techTreeSystem->getResearchPoints() + 100);
                    std::cout << "Added 100 research points. Total: " 
                             << techTreeSystem->getResearchPoints() << std::endl;
                    break;
            }
        }
        
        // Pass events to UI components
        if (uiManager) {
            uiManager->handleEvent(event);
        }
    }
    
    void update() {
        if (techTreeSystem) {
            techTreeSystem->update(0.016f); // 60 FPS
        }
    }
    
    void render() {
        SDL_SetRenderDrawColor(sdlManager.getRenderer(), 20, 20, 30, 255);
        SDL_RenderClear(sdlManager.getRenderer());
        
        // Render title
        if (uiManager) {
            // Simple text rendering for title - in real implementation you'd use proper font rendering
            // uiManager->renderText("Tech Tree UILabel Test", 10, 10, {255, 255, 255, 255});
            uiManager->renderAll();
        }
        
        SDL_RenderPresent(sdlManager.getRenderer());
    }
    
    void run() {
        std::cout << "\n🚀 Starting UILabel Tech Tree Test..." << std::endl;
        
        SDL_Event event;
        while (running) {
            // Handle events
            while (SDL_PollEvent(&event)) {
                handleEvent(event);
            }
            
            // Update
            update();
            
            // Render
            render();
            
            // Limit frame rate
            SDL_Delay(16); // ~60 FPS
        }
        
        std::cout << "\n✅ UILabel Tech Tree Test completed!" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        TechTreeUITest test;
        test.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
