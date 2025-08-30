#pragma once
#include <string>

struct Card {
    std::string name;
    int rarity; // 1=Common, 2=Rare, 3=Legendary
    int quantity;
    Card(const std::string& n, int r, int q = 1) : name(n), rarity(r), quantity(q) {}
};