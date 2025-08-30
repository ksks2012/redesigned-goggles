#include "Controller.h"
#include "Constants.h"
#include <random>
#include <thread>
#include <iostream>

Controller::Controller(Inventory& inv, View& v) : inventory(inv), view(v), selectedCard(nullptr), running(true) {}

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
        }
    }
}

bool Controller::isRunning() const {
    return running;
}

void Controller::updateView() {
    view.render(inventory, selectedCard, mouseX, mouseY);
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
            inventory.addCard(Card(Constants::CARD_NAMES[nameDist(gen)], rarityDist(gen)));
        }
        std::this_thread::sleep_for(Constants::ORGANIZE_INTERVAL);
    }
}

void Controller::handleMouseDown(int x, int y) {
    if (x >= Constants::BUTTON_X && x <= Constants::BUTTON_MAX_X && y >= Constants::BUTTON_Y_ADD && y <= Constants::BUTTON_Y_ADD_END) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> rarityDist(Constants::RARITY_MIN, Constants::RARITY_MAX);
        std::uniform_int_distribution<> nameDist(0, Constants::CARD_NAMES.size() - 1);
        inventory.addCard(Card(Constants::CARD_NAMES[nameDist(gen)], rarityDist(gen)));
    } else if (x >= Constants::BUTTON_X && x <= Constants::BUTTON_MAX_X && y >= Constants::BUTTON_Y_REMOVE && y <= Constants::BUTTON_Y_REMOVE_END) {
        auto& cards = inventory.getCards();
        if (!cards.empty()) {
            inventory.removeCard(cards[0].name, cards[0].rarity);
        }
    } else if (x >= Constants::BUTTON_X && x <= Constants::BUTTON_MAX_X && y >= Constants::BUTTON_Y_EXPLORE && y <= Constants::BUTTON_Y_EXPLORE_END) {
        handleExplore();
    }

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