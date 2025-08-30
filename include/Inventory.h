#pragma once
#include <vector>
#include <mutex>
#include "Card.h"

class Inventory {
public:
    void addCard(const Card& card);
    void removeCard(const std::string& name, int rarity);
    void updateCards(const std::vector<Card>& newCards);
    const std::vector<Card>& getCards() const;

private:
    std::vector<Card> cards;
    std::mutex mutex;
};