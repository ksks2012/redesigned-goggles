#include "Controller.h"
#include "Constants.h"
#include <random>
#include <thread>
#include <iostream>

Controller::Controller(Inventory& inv, View& v, CraftingSystem& crafting) 
    : inventory_(inv), view_(v), craftingSystem_(crafting) {
    
    // Create input handler with view interface
    inputHandler_ = std::make_unique<GameInputHandler>(view_, inventory_, craftingSystem_);
    
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
            inputHandler_->handleMouseWheel(event.wheel.x, event.wheel.y, event.wheel.y);
            break;
        case SDL_KEYDOWN:
            inputHandler_->handleKeyDown(event.key.keysym.sym);
            break;
    }
}

bool Controller::isRunning() const {
    return inputHandler_->isRunning();
}

void Controller::updateView() {
    view_.render(inventory_, inputHandler_->getSelectedCard(), 
                inputHandler_->getMouseX(), inputHandler_->getMouseY(), 
                inputHandler_->isShowingCraftingPanel(), craftingSystem_,
                inputHandler_->getInventoryScrollOffset(), inputHandler_->getCraftingScrollOffset());
}

void Controller::organizeInventory() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rarityDist(Constants::RARITY_MIN, Constants::RARITY_MAX);

    while (isRunning()) {
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
        std::this_thread::sleep_for(Constants::ORGANIZE_INTERVAL);
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
                inventory_.removeCard(card.name, card.rarity);
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