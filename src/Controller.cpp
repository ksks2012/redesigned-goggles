#include "Controller.h"
#include "Constants.h"
#include <random>
#include <thread>
#include <iostream>

Controller::Controller(Inventory& inv, View& v, CraftingSystem& crafting) 
    : inventory(inv), view(v), craftingSystem(crafting), selectedCard(nullptr), running(true) {}

void Controller::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    handleMouseDown(event.button.x, event.button.y);
                }
                break;
            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    selectedCard = nullptr;
                }
                break;
            case SDL_KEYDOWN:
                handleKeyDown(event.key.keysym.sym);
                break;
        }
    }
}

bool Controller::isRunning() const {
    return running;
}

void Controller::updateView() {
    view.render(inventory, selectedCard, mouseX, mouseY, showCraftingPanel, craftingSystem);
}

void Controller::organizeInventory() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rarityDist(Constants::RARITY_MIN, Constants::RARITY_MAX);
    std::uniform_int_distribution<> nameDist(0, Constants::CARD_NAMES.size() - 1);

    while (running) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            std::vector<Card> newCards;
            for (const auto& card : inventory.getCards()) {
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
            inventory.updateCards(newCards);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> rarityDist(Constants::RARITY_MIN, Constants::RARITY_MAX);
            inventory.addCard(Constants::RandomCardGenerator::generateRandomCardByRarity(rarityDist(gen)));
        }
        std::this_thread::sleep_for(Constants::ORGANIZE_INTERVAL);
    }
}

void Controller::handleMouseDown(int x, int y) {
    if (x >= Constants::BUTTON_X && x <= Constants::BUTTON_MAX_X && y >= Constants::BUTTON_Y_ADD && y <= Constants::BUTTON_Y_ADD_END) {
        // Add card button
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> rarityDist(Constants::RARITY_MIN, Constants::RARITY_MAX);
        inventory.addCard(Constants::RandomCardGenerator::generateRandomCardByRarity(rarityDist(gen)));
        } else if (x >= Constants::BUTTON_X && x <= Constants::BUTTON_MAX_X && y >= Constants::BUTTON_Y_REMOVE && y <= Constants::BUTTON_Y_REMOVE_END) {
        // Remove card button
        auto& cards = inventory.getCards();
        if (!cards.empty()) {
            inventory.removeCard(cards[0].name, cards[0].rarity);
        }
        } else if (x >= Constants::BUTTON_X && x <= Constants::BUTTON_MAX_X && y >= Constants::BUTTON_Y_EXPLORE && y <= Constants::BUTTON_Y_EXPLORE_END) {
        // Explore button
        handleExplore();
        } else if (x >= Constants::BUTTON_X && x <= Constants::BUTTON_MAX_X && y >= Constants::BUTTON_Y_CRAFT && y <= Constants::BUTTON_Y_CRAFT_END) {
        // Crafting button
        showCraftingPanel = !showCraftingPanel;
        std::cout << (showCraftingPanel ? "Crafting panel opened" : "Crafting panel closed") << std::endl;
        } else if (showCraftingPanel) {
        // Handle clicks inside crafting panel
        handleCrafting();
        return;
        }

        // Card selection logic (only when crafting panel is not shown)
    if (!showCraftingPanel) {
        int index = 0;
        for (const auto& card : inventory.getCards()) {
            int cardY = Constants::CARD_X + index * Constants::CARD_SPACING;
            if (x >= Constants::CARD_X && x <= Constants::CARD_MAX_X && y >= cardY && y <= cardY + Constants::CARD_HEIGHT) {
                selectedCard = const_cast<Card*>(&card);
                return;
            }
            index++;
        }
        selectedCard = nullptr;
    }
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
                inventory.addCard(card);
                std::cout << "Event: " << event.description << " - Gained " << card.name << " x" << card.quantity << std::endl;
            }
            for (const auto& card : event.penalties) {
                inventory.removeCard(card.name, card.rarity);
                std::cout << "Event: " << event.description << " - Lost " << card.name << " x" << card.quantity << std::endl;
            }
            break;
        }
    }
}

void Controller::setSaveCallback(std::function<bool()> saveCallback) {
    this->saveCallback = saveCallback;
}

void Controller::setLoadCallback(std::function<bool()> loadCallback) {
    this->loadCallback = loadCallback;
}

void Controller::handleKeyDown(SDL_Keycode key) {
    switch (key) {
        case SDLK_s:
            // Press S to save (simplified, not Ctrl+S)
            if (saveCallback) {
                if (saveCallback()) {
                    std::cout << "Game saved manually!" << std::endl;
                } else {
                    std::cout << "Save failed!" << std::endl;
                }
            }
            break;

        case SDLK_l:
            // Press L to load
            if (loadCallback) {
                if (loadCallback()) {
                    std::cout << "Game loaded!" << std::endl;
                } else {
                    std::cout << "Load failed!" << std::endl;
                }
            }
            break;
            
        case SDLK_c:
            // Press C to toggle crafting panel
            showCraftingPanel = !showCraftingPanel;
            std::cout << (showCraftingPanel ? "Crafting panel opened" : "Crafting panel closed") << std::endl;
            break;

        case SDLK_ESCAPE:
            // ESC to exit game
            running = false;
            break;

        default:
            break;
    }
}

void Controller::handleCrafting() {
    // Simplified crafting panel click handling
    // Only handles recipe selection here; actual UI click detection is handled in View

    // Get available recipes
    auto availableRecipes = craftingSystem.getAvailableRecipes(inventory);

    if (availableRecipes.empty()) {
        std::cout << "No available crafting recipes" << std::endl;
        return;
    }

    // Simple recipe selection logic (can be extended to more complex UI later)
    // Uses mouse position to select recipe
    int recipeIndex = (mouseY - Constants::CRAFT_PANEL_Y - Constants::CRAFT_PANEL_RECIPE_LIST_OFFSET) / Constants::RECIPE_ITEM_HEIGHT;

    if (recipeIndex >= 0 && recipeIndex < static_cast<int>(availableRecipes.size())) {
        craftSelectedRecipe(availableRecipes[recipeIndex]);
    }
}

void Controller::craftSelectedRecipe(const Recipe& recipe) {
    std::cout << "Attempting to craft: " << recipe.name << std::endl;

    // Check if crafting is possible
    if (!craftingSystem.canCraft(recipe, inventory)) {
        std::cout << "Cannot craft " << recipe.name << " - insufficient materials or recipe not unlocked" << std::endl;
        return;
    }

    // Perform crafting
    CraftingResult result = craftingSystem.craftItem(recipe, inventory);

    if (result.success) {
        std::cout << "Crafting successful! Received: " << result.resultCard.name << std::endl;
    } else {
        std::cout << "Crafting failed: " << result.message << std::endl;
    }
}