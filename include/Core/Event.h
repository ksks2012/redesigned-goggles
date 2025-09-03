#pragma once
#include <string>
#include <vector>
#include "Core/Card.h"

struct Event {
    std::string description; // e.g., "Found abandoned warehouse"
    std::vector<Card> rewards; // e.g., {Card("Wood", 1, 3)}
    std::vector<Card> penalties; // e.g., {Card("Food", 1, 1)}
    float probability; // Event trigger probability
    Event(const std::string& desc, const std::vector<Card>& r, const std::vector<Card>& p, float prob)
        : description(desc), rewards(r), penalties(p), probability(prob) {}
};