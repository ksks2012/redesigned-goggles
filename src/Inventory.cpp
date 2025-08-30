#include "Inventory.h"

void Inventory::addCard(const Card& card) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& c : cards) {
        if (c.name == card.name && c.rarity == card.rarity) {
            c.quantity += card.quantity;
            return;
        }
    }
    cards.push_back(card);
}

void Inventory::removeCard(const std::string& name, int rarity) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto it = cards.begin(); it != cards.end(); ++it) {
        if (it->name == name && it->rarity == rarity) {
            if (it->quantity > 1) {
                it->quantity--;
            } else {
                cards.erase(it);
            }
            return;
        }
    }
}

void Inventory::updateCards(const std::vector<Card>& newCards) {
    std::lock_guard<std::mutex> lock(mutex);
    cards = newCards;
}

const std::vector<Card>& Inventory::getCards() const {
    return cards;
}