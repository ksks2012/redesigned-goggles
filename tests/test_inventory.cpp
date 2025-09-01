#include "../lib/catch2/catch.hpp"
#include "../include/Inventory.h"
#include "../include/Card.h"
#include <thread>
#include <chrono>

TEST_CASE("Inventory basic operations", "[Inventory]") {
    Inventory inventory;
    
    SECTION("Adding cards to inventory") {
        Card ironOre("Iron Ore", 1, CardType::METAL, 10);
        Card wood("Wood", 1, CardType::BUILDING, 5);
        
        inventory.addCard(ironOre);
        inventory.addCard(wood);
        
        const auto& cards = inventory.getCards();
        REQUIRE(cards.size() == 2);
        REQUIRE(cards[0].name == "Iron Ore");
        REQUIRE(cards[0].quantity == 10);
        REQUIRE(cards[1].name == "Wood");
        REQUIRE(cards[1].quantity == 5);
    }
    
    SECTION("Adding duplicate cards increases quantity") {
        Card ironOre1("Iron Ore", 1, CardType::METAL, 5);
        Card ironOre2("Iron Ore", 1, CardType::METAL, 3);
        
        inventory.addCard(ironOre1);
        inventory.addCard(ironOre2);
        
        const auto& cards = inventory.getCards();
        REQUIRE(cards.size() == 1);
        REQUIRE(cards[0].name == "Iron Ore");
        REQUIRE(cards[0].quantity == 8);
    }
    
    SECTION("Removing cards from inventory") {
        Card weapon("Sword", 2, CardType::WEAPON, 2);
        inventory.addCard(weapon);
        
        // Remove one sword
        inventory.removeCard("Sword", 2);
        
        const auto& cards = inventory.getCards();
        REQUIRE(cards.size() == 1);
        REQUIRE(cards[0].quantity == 1);
        
        // Remove the last sword
        inventory.removeCard("Sword", 2);
        
        REQUIRE(inventory.getCards().empty());
    }
    
    SECTION("Removing non-existent card does nothing") {
        Card food("Apple", 1, CardType::FOOD, 3);
        inventory.addCard(food);
        
        size_t originalSize = inventory.getCards().size();
        inventory.removeCard("Orange", 1); // Non-existent card
        
        REQUIRE(inventory.getCards().size() == originalSize);
        REQUIRE(inventory.getCards()[0].name == "Apple");
    }
    
    SECTION("Updating entire card collection") {
        Card oldCard1("Old Item 1", 1, CardType::MISC, 5);
        Card oldCard2("Old Item 2", 2, CardType::MISC, 3);
        inventory.addCard(oldCard1);
        inventory.addCard(oldCard2);
        
        // Create new card collection
        std::vector<Card> newCards = {
            Card("New Item 1", 1, CardType::TOOL, 2),
            Card("New Item 2", 3, CardType::WEAPON, 1),
            Card("New Item 3", 1, CardType::FOOD, 10)
        };
        
        inventory.updateCards(newCards);
        
        const auto& cards = inventory.getCards();
        REQUIRE(cards.size() == 3);
        REQUIRE(cards[0].name == "New Item 1");
        REQUIRE(cards[1].name == "New Item 2");
        REQUIRE(cards[2].name == "New Item 3");
    }
}

TEST_CASE("Inventory thread safety", "[Inventory][threading]") {
    Inventory inventory;
    
    SECTION("Concurrent adding and reading") {
        const int numThreads = 4;
        const int cardsPerThread = 25;
        
        std::vector<std::thread> threads;
        
        // Create threads that add cards concurrently
        for (int t = 0; t < numThreads; ++t) {
            threads.emplace_back([&inventory, t, cardsPerThread]() {
                for (int i = 0; i < cardsPerThread; ++i) {
                    Card card("Thread" + std::to_string(t) + "_Item" + std::to_string(i), 
                             1, CardType::MISC, 1);
                    inventory.addCard(card);
                }
            });
        }
        
        // Create a reader thread
        std::vector<size_t> sizes;
        threads.emplace_back([&inventory, &sizes]() {
            for (int i = 0; i < 10; ++i) {
                sizes.push_back(inventory.getCards().size());
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Verify final state
        const auto& cards = inventory.getCards();
        REQUIRE(cards.size() == numThreads * cardsPerThread);
        
        // Verify that reading was safe (no crashes or invalid data)
        REQUIRE(sizes.size() == 10);
        for (size_t size : sizes) {
            REQUIRE(size <= numThreads * cardsPerThread);
        }
    }
    
    SECTION("Concurrent removing and adding") {
        // Pre-populate inventory
        for (int i = 0; i < 50; ++i) {
            Card card("Item" + std::to_string(i), 1, CardType::MISC, 2);
            inventory.addCard(card);
        }
        
        std::vector<std::thread> threads;
        
        // Thread that adds cards
        threads.emplace_back([&inventory]() {
            for (int i = 50; i < 75; ++i) {
                Card card("NewItem" + std::to_string(i), 1, CardType::MISC, 1);
                inventory.addCard(card);
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
        
        // Thread that removes cards
        threads.emplace_back([&inventory]() {
            for (int i = 0; i < 25; ++i) {
                inventory.removeCard("Item" + std::to_string(i), 1);
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
        
        // Wait for completion
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Verify inventory is in a consistent state
        const auto& cards = inventory.getCards();
        REQUIRE(cards.size() > 0); // Should have some cards remaining
        
        // Check that all remaining cards have valid data
        for (const auto& card : cards) {
            REQUIRE(!card.name.empty());
            REQUIRE(card.quantity >= 0);
        }
    }
}

TEST_CASE("Inventory edge cases", "[Inventory]") {
    Inventory inventory;
    
    SECTION("Adding card with zero quantity") {
        Card zeroCard("Zero Item", 1, CardType::MISC, 0);
        inventory.addCard(zeroCard);
        
        const auto& cards = inventory.getCards();
        REQUIRE(cards.size() == 1);
        REQUIRE(cards[0].quantity == 0);
    }
    
    SECTION("Adding card with negative quantity") {
        Card negativeCard("Negative Item", 1, CardType::MISC, -5);
        inventory.addCard(negativeCard);
        
        const auto& cards = inventory.getCards();
        REQUIRE(cards.size() == 1);
        REQUIRE(cards[0].quantity == -5);
    }
    
    SECTION("Empty inventory operations") {
        REQUIRE(inventory.getCards().empty());
        
        // Try to remove from empty inventory
        inventory.removeCard("Non-existent", 1);
        REQUIRE(inventory.getCards().empty());
        
        // Update with empty collection
        std::vector<Card> emptyCards;
        inventory.updateCards(emptyCards);
        REQUIRE(inventory.getCards().empty());
    }
    
    SECTION("Large inventory stress test") {
        const int numCards = 1000;
        
        // Add many cards
        for (int i = 0; i < numCards; ++i) {
            Card card("Item" + std::to_string(i), 
                     (i % 3) + 1, // Rarity 1-3
                     static_cast<CardType>(i % 9), // Cycle through card types
                     (i % 10) + 1); // Quantity 1-10
            inventory.addCard(card);
        }
        
        REQUIRE(inventory.getCards().size() == numCards);
        
        // Remove half of them (considering each removeCard call removes only 1 quantity)
        int expectedRemaining = 0;
        for (int i = 0; i < numCards / 2; ++i) {
            int originalQuantity = (i % 10) + 1;
            // Remove one item at a time, but items may have quantity > 1
            inventory.removeCard("Item" + std::to_string(i), (i % 3) + 1);
            if (originalQuantity > 1) {
                expectedRemaining++; // Item still exists with reduced quantity
            }
            // If originalQuantity was 1, item is completely removed
        }
        
        // Count remaining items
        size_t actualRemaining = 0;
        for (const auto& card : inventory.getCards()) {
            if (card.name.find("Item") == 0) {
                int itemNum = std::stoi(card.name.substr(4));
                if (itemNum >= numCards / 2) {
                    actualRemaining++; // Items from second half should still be there
                } else {
                    actualRemaining++; // Items from first half that had quantity > 1
                }
            }
        }
        
        REQUIRE(inventory.getCards().size() >= numCards / 2); // At least half should remain
        REQUIRE(inventory.getCards().size() <= numCards); // But not more than original
    }
}
