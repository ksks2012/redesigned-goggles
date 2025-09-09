#include "Core/Controller.h"
#include "Constants.h"
#include <random>
#include <thread>
#include <iostream>

Controller::Controller(Inventory& inv, View& v, CraftingSystem& crafting, BaseManager& baseManager) 
    : inventory_(inv), view_(v), craftingSystem_(crafting), baseManager_(baseManager) {
    
    // Create base building controller
    baseBuildingController_ = std::make_shared<BaseBuildingController>(baseManager, inventory_);
    
    // Set up notification callback for user feedback
    baseBuildingController_->setNotificationCallback([this](const std::string& message) {
        std::cout << "[Base Building] " << message << std::endl;
    });
    
    // Start durability decay system
    baseBuildingController_->startDurabilityDecay();
    
    // Create input handler with base building support
    inputHandler_ = std::make_unique<GameInputHandler>(view_, inventory_, craftingSystem_, baseBuildingController_);
    
    // Set up callbacks for input handler
    inputHandler_->setExploreCallback([this]() { handleExplore(); });
}

void Controller::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        handleEvent(event);
    }
}

void Controller::handleEvent(SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT:
            inputHandler_->handleQuit();
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                inputHandler_->handleMouseDown(event.button.x, event.button.y);
            }
            break;
        case SDL_MOUSEMOTION:
            inputHandler_->handleMouseMotion(event.motion.x, event.motion.y);
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                inputHandler_->handleMouseUp(event.button.x, event.button.y);
            }
            break;
        case SDL_MOUSEWHEEL:
            inputHandler_->handleMouseWheel(inputHandler_->getMouseX(), inputHandler_->getMouseY(), event.wheel.y);
            break;
        case SDL_KEYDOWN:
            inputHandler_->handleKeyDown(event.key.keysym.sym);
            break;
    }
    static Uint32 lastClickTime = 0;
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastClickTime < Constants::CLICK_INTERVAL_MS) {
            // Ignore this click, too soon after previous
            return;
        }
        lastClickTime = currentTime;
    }
}

bool Controller::isRunning() const {
    return inputHandler_->isRunning();
}

void Controller::updateView() {
    // Validate card pointers before rendering to prevent dangling pointer errors
    // FIXME:
    // inputHandler_->validateCardPointers();
    
    view_.render(inventory_, inputHandler_->getSelectedCard(), 
                inputHandler_->getMouseX(), inputHandler_->getMouseY(), 
                inputHandler_->isShowingCraftingPanel(), craftingSystem_,
                inputHandler_->getInventoryScrollOffset(), inputHandler_->getCraftingScrollOffset(),
                inputHandler_->isDragging(), inputHandler_->getDraggedCard());
}

void Controller::organizeInventory() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rarityDist(Constants::RARITY_MIN, Constants::RARITY_MAX);

    while (isRunning() && organizeInventoryEnabled_) {
        // Check if organizeInventory is enabled before processing
        if (organizeInventoryEnabled_) {
            std::lock_guard<std::mutex> lock(mutex_);
            std::vector<Card> newCards;
            for (const auto& card : inventory_.getCards()) {
                bool merged = false;
                for (auto& newCard : newCards) {
                    if (newCard.name == card.name && newCard.rarity == card.rarity) {
                        newCard.quantity += card.quantity;
                        merged = true;
                        break;
                    }
                }
                if (!merged) newCards.push_back(card);
            }
            inventory_.updateCards(newCards);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> rarityDist(Constants::RARITY_MIN, Constants::RARITY_MAX);
            inventory_.addCard(Constants::RandomCardGenerator::generateRandomCardByRarity(rarityDist(gen)));
        }
        
        // Use much shorter sleep intervals for faster shutdown response
        // Split the total sleep time into smaller chunks for immediate response
        const auto totalSleepTime = Constants::ORGANIZE_INTERVAL;
        const auto shortInterval = std::chrono::milliseconds(50); // Check every 50ms instead of 100ms
        auto elapsed = std::chrono::milliseconds(0);
        
        while (elapsed < totalSleepTime && isRunning() && organizeInventoryEnabled_) {
            std::this_thread::sleep_for(shortInterval);
            elapsed += shortInterval;
            
            // Early exit check every 200ms for even faster response
            if (elapsed.count() % 200 == 0 && (!isRunning() || !organizeInventoryEnabled_)) {
                break;
            }
        }
    }
}

void Controller::setSaveCallback(std::function<bool()> saveCallback) {
    saveCallback_ = saveCallback;
    inputHandler_->setSaveCallback(saveCallback);
}

void Controller::setLoadCallback(std::function<bool()> loadCallback) {
    loadCallback_ = loadCallback;
    inputHandler_->setLoadCallback(loadCallback);
}

void Controller::handleExplore() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0f, 1.0f);
    float roll = dist(gen);
    float cumulative = 0.0f;
    for (const auto& event : Constants::EXPLORATION_EVENTS) {
        cumulative += event.probability;
        if (roll <= cumulative) {
            for (const auto& card : event.rewards) {
                inventory_.addCard(card);
                std::cout << "Event: " << event.description << " - Gained " << card.name << " x" << card.quantity << std::endl;
            }
            for (const auto& card : event.penalties) {
                safeRemoveCard(card.name, card.rarity);
                std::cout << "Event: " << event.description << " - Lost " << card.name << " x" << card.quantity << std::endl;
            }
            break;
        }
    }
}

void Controller::pauseOrganizeInventory() {
    std::lock_guard<std::mutex> lock(mutex_);
    organizeInventoryEnabled_ = false;
    std::cout << "Inventory organization paused for editor mode" << std::endl;
}

void Controller::resumeOrganizeInventory() {
    std::lock_guard<std::mutex> lock(mutex_);
    organizeInventoryEnabled_ = true;
    std::cout << "Inventory organization resumed" << std::endl;
}

void Controller::stopOrganizeInventory() {
    std::lock_guard<std::mutex> lock(mutex_);
    organizeInventoryEnabled_ = false;
    std::cout << "Inventory organization stopped permanently" << std::endl;
}

void Controller::safeRemoveCard(const std::string& name, int rarity) {
    // Find the card that will be removed to check if it's selected
    const auto& cards = inventory_.getCards();
    const Card* cardToRemove = nullptr;
    
    for (const auto& card : cards) {
        if (card.name == name && card.rarity == rarity) {
            cardToRemove = &card;
            break;
        }
    }
    
    // Clear selection state if the card being removed is selected
    if (cardToRemove) {
        inputHandler_->validateCardPointers();  // This will clean up any invalid pointers
    }
    
    // Remove the card from inventory
    inventory_.removeCard(name, rarity);
    
    // Validate pointers again after removal to ensure consistency
    inputHandler_->validateCardPointers();
}