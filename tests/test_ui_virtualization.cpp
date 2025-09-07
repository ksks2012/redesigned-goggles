#include "Core/Inventory.h"
#include "Core/Card.h"
#include <catch2/catch.hpp>
#include <iostream>

TEST_CASE("Inventory Virtualization Logic", "[ui][virtualization]") {
    SECTION("Large inventory handling") {
        Inventory testInventory;
        
        // Create large inventory for testing virtualization scenario
        for (int i = 0; i < 100; ++i) {
            Card testCard("TestCard" + std::to_string(i), 1, CardType::MISC, 1);
            testInventory.addCard(testCard);
        }
        
        REQUIRE(testInventory.getCards().size() == 100);
        
        // Test that we can access cards by index (simulating virtualization)
        const auto& cards = testInventory.getCards();
        REQUIRE(cards[0].name == "TestCard0");
        REQUIRE(cards[50].name == "TestCard50");
        REQUIRE(cards[99].name == "TestCard99");
    }

    SECTION("Scroll range calculation simulation") {
        int totalItems = 100;
        int visibleItems = 10;
        int bufferItems = 2;
        
        // Simulate different scroll positions
        auto calculateRange = [&](int scrollOffset) -> std::pair<int, int> {
            int cardSpacing = 40;
            int startIndex = std::max(0, (scrollOffset / cardSpacing) - bufferItems);
            int endIndex = std::min(totalItems, startIndex + visibleItems + (2 * bufferItems));
            return {startIndex, endIndex};
        };
        
        // Test scroll at top
        auto [start1, end1] = calculateRange(0);
        REQUIRE(start1 == 0);
        REQUIRE(end1 <= visibleItems + (2 * bufferItems));
        
        // Test scroll in middle
        auto [start2, end2] = calculateRange(500);
        REQUIRE(start2 > 0);
        REQUIRE(end2 > start2);
        REQUIRE((end2 - start2) <= visibleItems + (2 * bufferItems));
        
        // Test scroll near bottom
        auto [start3, end3] = calculateRange(3600); // Close to end
        REQUIRE(start3 < totalItems);
        REQUIRE(end3 == totalItems);
    }

    SECTION("Card key generation for selection state") {
        Card card1("TestCard", 1, CardType::MISC, 1);
        Card card2("TestCard", 2, CardType::MISC, 1);  // Different rarity
        Card card3("DifferentCard", 1, CardType::MISC, 1);  // Different name
        
        auto getCardKey = [](const Card& card) {
            return card.name + "_" + std::to_string(card.rarity);
        };
        
        REQUIRE(getCardKey(card1) == "TestCard_1");
        REQUIRE(getCardKey(card2) == "TestCard_2");
        REQUIRE(getCardKey(card3) == "DifferentCard_1");
        
        // Keys should be unique for different cards
        REQUIRE(getCardKey(card1) != getCardKey(card2));
        REQUIRE(getCardKey(card1) != getCardKey(card3));
    }
}
