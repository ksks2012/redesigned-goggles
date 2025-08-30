#include "Controller.h"
#include <random>
#include <thread>
#include <chrono>
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
    std::vector<std::string> cardNames = {"Wood", "Metal", "Food", "Water", "Medicine", "Weapon"};
    std::uniform_int_distribution<> rarityDist(1, 3);
    std::uniform_int_distribution<> nameDist(0, cardNames.size() - 1);

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
            inventory.addCard(Card(cardNames[nameDist(gen)], rarityDist(gen)));
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void Controller::handleMouseDown(int x, int y) {
    if (x >= 600 && x <= 700 && y >= 50 && y <= 90) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<std::string> cardNames = {"Wood", "Metal", "Food", "Water", "Medicine", "Weapon"};
        std::uniform_int_distribution<> rarityDist(1, 3);
        std::uniform_int_distribution<> nameDist(0, cardNames.size() - 1);
        inventory.addCard(Card(cardNames[nameDist(gen)], rarityDist(gen)));
    } else if (x >= 600 && x <= 700 && y >= 100 && y <= 140) {
        auto& cards = inventory.getCards();
        if (!cards.empty()) {
            inventory.removeCard(cards[0].name, cards[0].rarity);
        }
    } else if (x >= 600 && x <= 700 && y >= 150 && y <= 190) { // Explore button
        handleExplore();
    }

    int index = 0;
    for (const auto& card : inventory.getCards()) {
        int cardY = 50 + index * 60;
        if (x >= 50 && x <= 250 && y >= cardY && y <= cardY + 50) {
            selectedCard = const_cast<Card*>(&card);
            return;
        }
        index++;
    }
    selectedCard = nullptr;
}

void Controller::handleExplore() {
    static const std::vector<Event> events = {
        Event("Found Abandoned Warehouse", {Card("Wood", 1, 3)}, {}, 0.4f),
        Event("Zombie Attack", {}, {Card("Food", 1, 1)}, 0.3f),
        Event("Medical Kit Found", {Card("Medicine", 2, 1)}, {}, 0.2f),
        Event("Water Source Discovered", {Card("Water", 1, 2)}, {}, 0.1f)
    };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0f, 1.0f);
    float roll = dist(gen);
    float cumulative = 0.0f;
    for (const auto& event : events) {
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