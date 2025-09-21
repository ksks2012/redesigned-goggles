#include "Systems/TechTreeSystem.h"
#include "Systems/DataManager.h"
#include "Systems/SDLManager.h"
#include "Interface/ui/UIManager.h"
#include <iostream>

/**
 * @brief Interactive Tech Tree Layout Demo
 * Shows the automatic UILabel positioning and connection lines
 */
class TechTreeLayoutDemo {
private:
    SDLManager sdlManager;
    DataManagement::GameDataManager dataManager;
    std::unique_ptr<TechTreeSystem> techTreeSystem;
    std::shared_ptr<UIManager> uiManager;
    std::shared_ptr<TechTreeUI> techTreeUI;
    
    bool running = true;
    int windowWidth = 1000;
    int windowHeight = 800;
    
public:
    TechTreeLayoutDemo(int width = 1000, int height = 800) : windowWidth(width), windowHeight(height) {
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
        
        // Create a clean tech tree for demo (bypass default techs)
        TechTree* techTree = techTreeSystem->getTechTree();
        
        // Clear any existing techs by deserializing an empty tree
        techTree->deserializeFromJson("{\"techs\":{}}");
        
        // Now create our comprehensive test tech tree to demonstrate layout
        
        // Level 0 - Root technologies
        auto basicSurvival = std::make_shared<TechNode>(
            "basic_survival", "Basic Survival", 
            "Essential survival skills", TechType::SURVIVAL, 50, 0, 0
        );
        basicSurvival->status = TechStatus::AVAILABLE;
        
        // Level 1 - Primary branches
        auto basicFarming = std::make_shared<TechNode>(
            "basic_farming", "Basic Agriculture", 
            "Learn to grow crops", TechType::AGRICULTURE, 150, 0, 0
        );
        basicFarming->addPrerequisite("basic_survival", true);
        
        auto basicCrafting = std::make_shared<TechNode>(
            "basic_crafting", "Basic Crafting", 
            "Master basic crafting", TechType::CRAFTING, 100, 0, 0
        );
        basicCrafting->addPrerequisite("basic_survival", true);
        
        auto basicBuilding = std::make_shared<TechNode>(
            "basic_building", "Basic Construction", 
            "Build simple structures", TechType::BUILDING, 200, 0, 0
        );
        basicBuilding->addPrerequisite("basic_survival", true);
        
        // Level 2 - Advanced technologies
        auto advancedFarming = std::make_shared<TechNode>(
            "advanced_farming", "Advanced Agriculture", 
            "Irrigation and high-yield", TechType::AGRICULTURE, 350, 0, 0
        );
        advancedFarming->addPrerequisite("basic_farming", true);
        
        auto advancedCrafting = std::make_shared<TechNode>(
            "advanced_crafting", "Advanced Crafting", 
            "Metalworking and tools", TechType::CRAFTING, 300, 0, 0
        );
        advancedCrafting->addPrerequisite("basic_crafting", true);
        
        auto advancedBuilding = std::make_shared<TechNode>(
            "advanced_building", "Advanced Construction", 
            "Complex structures", TechType::BUILDING, 400, 0, 0
        );
        advancedBuilding->addPrerequisite("basic_building", true);
        advancedBuilding->addPrerequisite("basic_crafting", true); // Multi-prerequisite
        
        // Level 3 - Specialized technologies
        auto militaryTech = std::make_shared<TechNode>(
            "military_tech", "Military Technology", 
            "Weapons and defense", TechType::MILITARY, 500, 0, 0
        );
        militaryTech->addPrerequisite("advanced_crafting", true);
        militaryTech->addPrerequisite("advanced_building", true);
        
        auto scienceTech = std::make_shared<TechNode>(
            "science_tech", "Scientific Research", 
            "Advanced scientific advancement", TechType::ADVANCED, 600, 0, 0
        );
        scienceTech->addPrerequisite("advanced_crafting", true);
        scienceTech->addPrerequisite("advanced_farming", true);
        
        // Add all techs to the tree
        techTree->addTech(basicSurvival);
        techTree->addTech(basicFarming);
        techTree->addTech(basicCrafting);
        techTree->addTech(basicBuilding);
        techTree->addTech(advancedFarming);
        techTree->addTech(advancedCrafting);
        techTree->addTech(advancedBuilding);
        techTree->addTech(militaryTech);
        techTree->addTech(scienceTech);
        
        // Create UI with configurable size for testing different window sizes
        int uiWidth = windowWidth;   // Use the specified window size
        int uiHeight = windowHeight;
        
        techTreeUI = techTreeSystem->createUI(0, 0, uiWidth, uiHeight);
        if (techTreeUI) {
            uiManager->addComponent(techTreeUI);
            std::cout << "🎨 Tech Tree Layout Demo Created!" << std::endl;
            std::cout << "   - Automatic hierarchical positioning" << std::endl;
            std::cout << "   - Visual connection lines between prerequisites" << std::endl;
            std::cout << "   - Color-coded tech status" << std::endl;
        }
        
        // Set initial research points
        techTreeSystem->setResearchPoints(500);
        
        std::cout << "\n🎮 Layout Demo Controls:" << std::endl;
        std::cout << "   - Click tech nodes to select" << std::endl;
        std::cout << "   - Press ENTER to research selected tech" << std::endl;
        std::cout << "   - Press R to add 100 research points" << std::endl;
        std::cout << "   - Press SPACE to unlock prerequisite chain" << std::endl;
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
                    std::cout << "💰 Added 100 research points. Total: " 
                             << techTreeSystem->getResearchPoints() << std::endl;
                    break;
                case SDLK_SPACE:
                    // Auto-unlock prerequisite chain for demonstration
                    techTreeSystem->getTechTree()->unlockTech("basic_survival");
                    techTreeSystem->getTechTree()->unlockTech("basic_farming");
                    techTreeSystem->getTechTree()->unlockTech("basic_crafting");
                    std::cout << "🔓 Unlocked basic technologies for demo" << std::endl;
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
        // Dark background for better visibility
        SDL_SetRenderDrawColor(sdlManager.getRenderer(), 15, 15, 25, 255);
        SDL_RenderClear(sdlManager.getRenderer());
        
        // Render UI
        if (uiManager) {
            uiManager->renderAll();
        }
        
        SDL_RenderPresent(sdlManager.getRenderer());
    }
    
    void run() {
        std::cout << "\n🚀 Starting Tech Tree Layout Demo..." << std::endl;
        std::cout << "📊 Watch the automatic positioning and connection lines!" << std::endl;
        
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
        
        std::cout << "\n✨ Layout Demo completed!" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments for window size
        int width = 1000;
        int height = 800;
        
        if (argc >= 3) {
            width = std::atoi(argv[1]);
            height = std::atoi(argv[2]);
            std::cout << "Using custom window size: " << width << "x" << height << std::endl;
        }
        
        TechTreeLayoutDemo demo(width, height);
        demo.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
