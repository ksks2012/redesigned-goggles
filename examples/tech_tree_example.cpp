#include "Systems/TechTreeSystem.h"
#include "Systems/DataManager.h"
#include "Systems/SDLManager.h"
#include "Interface/ui/UIManager.h"
#include <iostream>

/**
 * @brief Tech Tree Example Program
 * Demonstrates how to integrate and use the tech tree system in a game
 */
class TechTreeExample {
private:
    SDLManager sdlManager;
    DataManagement::GameDataManager dataManager;
    std::unique_ptr<TechTreeSystem> techTreeSystem;
    std::shared_ptr<UIManager> uiManager;
    std::shared_ptr<TechTreeUI> techTreeUI;
    
    // Game state
    int playerResearchPoints = 200;  // Player's research points
    bool showTechTree = false;       // Whether to show tech tree interface

public:
    TechTreeExample() {
        // Initialize systems
        if (!sdlManager.initialize()) {
            throw std::runtime_error("Failed to initialize SDL");
        }
        
        // Note: SDLManager::initialize() in this codebase already creates the SDL window
        // and renderer. The original call to sdlManager.createWindow(...) was invalid
        // because SDLManager has no such member; if your SDLManager requires a separate
        // window creation step, add a createWindow(...) method to Systems/SDLManager.h.
        
        // Load game data: GameDataManager does not provide loadAllGameData().
        // Replace the following commented call with the correct API for your DataManager,
        // for example:
        //   dataManager.loadAllData();
        //   dataManager.initialize();
        // or call whatever method your DataManagement::GameDataManager exposes to load data.
        // If no explicit load call is required, you can leave this commented out.
        // dataManager.loadAllGameData();
        
        // Create UI manager
        uiManager = std::make_shared<UIManager>();
        
        // Initialize tech tree system
        initializeTechTreeSystem();
    }
    
    ~TechTreeExample() {
        sdlManager.cleanup();
    }
    
    void initializeTechTreeSystem() {
        // Create tech tree system
        techTreeSystem = std::make_unique<TechTreeSystem>(sdlManager, &dataManager);
        
        if (!techTreeSystem->initialize()) {
            throw std::runtime_error("Failed to initialize tech tree system");
        }
        
        // Set callback functions
        techTreeSystem->setOnTechCompleted([this](const std::string& techId, const std::vector<TechReward>& rewards) {
            handleTechCompleted(techId, rewards);
        });
        
        techTreeSystem->setOnResourceConsume([this](int cost) -> bool {
            return consumeResearchPoints(cost);
        });
        
        // Set initial research points
        techTreeSystem->setResearchPoints(playerResearchPoints);
        
        // Create tech tree UI (initially hidden)
        techTreeUI = techTreeSystem->createUI(50, 50, 1100, 700);
        if (techTreeUI) {
            techTreeUI->setVisible(false);
            uiManager->addComponent(techTreeUI);
        }
        
        std::cout << "Tech tree system initialization completed!" << std::endl;
        std::cout << "Press 'T' key to open/close tech tree interface" << std::endl;
        std::cout << "Current research points: " << playerResearchPoints << std::endl;
    }
    
    void handleTechCompleted(const std::string& techId, const std::vector<TechReward>& rewards) {
        std::cout << "\n🎉 Tech research completed!" << std::endl;
        std::cout << "Completed tech: " << techId << std::endl;
        std::cout << "Rewards received:" << std::endl;
        
        for (const auto& reward : rewards) {
            std::cout << "  - " << reward.type << ": " << reward.identifier;
            if (reward.amount > 1) {
                std::cout << " x" << reward.amount;
            }
            std::cout << std::endl;
            
            // Apply reward to game systems
            applyRewardToGame(reward);
        }
        
        std::cout << "Current research points: " << techTreeSystem->getResearchPoints() << std::endl;
        std::cout << std::endl;
    }
    
    void applyRewardToGame(const TechReward& reward) {
        if (reward.type == "recipe") {
            // Unlock new recipe
            std::cout << "    ✅ Unlocked recipe: " << reward.identifier << std::endl;
            // dataManager.unlockRecipe(reward.identifier);
            
        } else if (reward.type == "building") {
            // Unlock new building
            std::cout << "    🏗️ Unlocked building: " << reward.identifier << std::endl;
            // dataManager.unlockBuilding(reward.identifier);
            
        } else if (reward.type == "passive") {
            // Apply passive effect
            std::cout << "    ⚡ Gained passive effect: " << reward.identifier << std::endl;
            
            if (reward.identifier == "research_speed_boost") {
                std::cout << "      Research speed increased!" << std::endl;
            }
        }
    }
    
    bool consumeResearchPoints(int cost) {
        if (playerResearchPoints >= cost) {
            playerResearchPoints -= cost;
            techTreeSystem->setResearchPoints(playerResearchPoints);
            std::cout << "Consumed research points: " << cost << ", remaining: " << playerResearchPoints << std::endl;
            return true;
        } else {
            std::cout << "Insufficient research points! Need: " << cost << ", current: " << playerResearchPoints << std::endl;
            return false;
        }
    }
    
    void handleKeyPress(SDL_Keycode key) {
        switch (key) {
            case SDLK_t:
                // Toggle tech tree display
                toggleTechTreeDisplay();
                break;
                
            case SDLK_r:
                // Add research points (cheat code)
                playerResearchPoints += 100;
                techTreeSystem->setResearchPoints(playerResearchPoints);
                std::cout << "Added 100 research points, current: " << playerResearchPoints << std::endl;
                break;
                
            case SDLK_s:
                // Save tech tree state
                saveTechTreeState();
                break;
                
            case SDLK_l:
                // Load tech tree state
                loadTechTreeState();
                break;
                
            case SDLK_ESCAPE:
                // Close tech tree interface
                if (showTechTree) {
                    toggleTechTreeDisplay();
                }
                break;
        }
    }
    
    void toggleTechTreeDisplay() {
        showTechTree = !showTechTree;
        if (techTreeUI) {
            techTreeUI->setVisible(showTechTree);
        }
        
        std::cout << (showTechTree ? "Opened tech tree interface" : "Closed tech tree interface") << std::endl;
        
        if (showTechTree) {
            printAvailableTechs();
        }
    }
    
    void printAvailableTechs() {
        auto techTree = techTreeSystem->getTechTree();
        if (!techTree) return;
        
        auto availableTechs = techTree->getAvailableTechs();
        auto researchingTechs = techTree->getResearchingTechs();
        
        std::cout << "\n📋 Available technologies for research:" << std::endl;
        for (const auto& tech : availableTechs) {
            std::cout << "  - " << tech->name << " (cost: " << tech->researchCost << ")" << std::endl;
        }
        
        if (!researchingTechs.empty()) {
            std::cout << "\n🔬 Currently researching:" << std::endl;
            for (const auto& tech : researchingTechs) {
                float progress = tech->getProgressPercent() * 100.0f;
                std::cout << "  - " << tech->name << " (" << progress << "%)" << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
    void saveTechTreeState() {
        if (techTreeSystem->saveTechTreeState("tech_tree_save.json")) {
            std::cout << "Tech tree state saved to tech_tree_save.json" << std::endl;
        } else {
            std::cout << "Failed to save tech tree state!" << std::endl;
        }
    }
    
    void loadTechTreeState() {
        if (techTreeSystem->loadTechTreeState("tech_tree_save.json")) {
            std::cout << "Tech tree state loaded from tech_tree_save.json" << std::endl;
        } else {
            std::cout << "Failed to load tech tree state!" << std::endl;
        }
    }
    
    void run() {
        SDL_Event event;
        bool running = true;
        Uint32 lastTime = SDL_GetTicks();
        
        std::cout << "\n🎮 Tech tree example running..." << std::endl;
        std::cout << "Control instructions:" << std::endl;
        std::cout << "  T - Open/Close tech tree" << std::endl;
        std::cout << "  R - Add 100 research points" << std::endl;
        std::cout << "  S - Save tech tree state" << std::endl;
        std::cout << "  L - Load tech tree state" << std::endl;
        std::cout << "  ESC - Close tech tree/Exit" << std::endl;
        std::cout << "  Q - Exit program" << std::endl;
        
        while (running) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;
            
            // Handle events
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        running = false;
                        break;
                        
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == SDLK_q) {
                            running = false;
                        } else {
                            handleKeyPress(event.key.keysym.sym);
                        }
                        break;
                }
                
                // Let UI handle events
                if (uiManager) {
                    uiManager->handleEvent(event);
                }
            }
            
            // Update systems
            if (techTreeSystem) {
                techTreeSystem->update(deltaTime);
            }
            
            // UIManager has no 'update' member; events are handled via handleEvent(...) and rendering via render().
            // If your UIManager provides a different update-like method, call it here (e.g., uiManager->tick();).
            if (uiManager) {
                // no-op
            }
            
            // Rendering
            SDL_SetRenderDrawColor(sdlManager.getRenderer(), 30, 30, 30, 255);
            SDL_RenderClear(sdlManager.getRenderer());
            
            if (uiManager) {
                uiManager->renderAll();
            }
            
            SDL_RenderPresent(sdlManager.getRenderer());
            
            // Limit frame rate
            SDL_Delay(16); // ~60 FPS
        }
        
        std::cout << "Tech tree example has exited." << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        TechTreeExample example;
        example.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
