#pragma once
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include "Core/Card.h"
#include "Core/Event.h"

namespace GameConstants {
// Game Logic Timing
constexpr std::chrono::seconds ORGANIZE_INTERVAL{10};
constexpr int FRAME_DELAY_MS = 16;

// Card System
constexpr int RARITY_MIN = 1;
constexpr int RARITY_MAX = 3;
inline const std::vector<std::string> CARD_NAMES = {"Wood", "Metal", "Food", "Water", "Medicine", "Weapon"};

// Grid Management System
constexpr int GRID_SIZE = 5;              // Initial grid size (5x5)
constexpr int MAX_GRID_SIZE = 10;         // Maximum expandable size
constexpr int INITIAL_UNLOCKED_SLOTS = 5; // Starting available slots

// Building System
constexpr int DURABILITY_DECAY_INTERVAL_MS = 60000; // 1 minute in milliseconds
constexpr float DURABILITY_DECAY_RATE = 0.01f;      // 1% per interval

// Card Factory - Create default cards with attributes
class CardFactory {
public:
    static Card createWood() {
        Card card("Wood", 1, CardType::BUILDING);
        card.setAttribute(AttributeType::WEIGHT, 2.0f);
        card.setAttribute(AttributeType::BURN_VALUE, 15.0f);
        card.setAttribute(AttributeType::CRAFTING_VALUE, 10.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 5.0f);
        return card;
    }

    static Card createMetal() {
        Card card("Metal", 2, CardType::METAL);
        card.setAttribute(AttributeType::WEIGHT, 5.0f);
        card.setAttribute(AttributeType::DURABILITY, 100.0f);
        card.setAttribute(AttributeType::CRAFTING_VALUE, 25.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 20.0f);
        return card;
    }

    static Card createFood() {
        Card card("Food", 1, CardType::FOOD);
        card.setAttribute(AttributeType::WEIGHT, 0.5f);
        card.setAttribute(AttributeType::NUTRITION, 20.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 8.0f);
        return card;
    }

    static Card createWater() {
        Card card("Water", 1, CardType::FOOD);
        card.setAttribute(AttributeType::WEIGHT, 1.0f);
        card.setAttribute(AttributeType::NUTRITION, 15.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 5.0f);
        return card;
    }

    static Card createMedicine() {
        Card card("Medicine", 2, CardType::HERB);
        card.setAttribute(AttributeType::WEIGHT, 0.2f);
        card.setAttribute(AttributeType::HEALING, 50.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 30.0f);
        return card;
    }

    static Card createWeapon() {
        Card card("Weapon", 3, CardType::WEAPON);
        card.setAttribute(AttributeType::WEIGHT, 3.0f);
        card.setAttribute(AttributeType::ATTACK, 45.0f);
        card.setAttribute(AttributeType::DURABILITY, 80.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 50.0f);
        return card;
    }

    static Card createIronOre() {
        Card card("Iron Ore", 2, CardType::METAL);
        card.setAttribute(AttributeType::WEIGHT, 4.0f);
        card.setAttribute(AttributeType::CRAFTING_VALUE, 20.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 15.0f);
        return card;
    }

    static Card createCoal() {
        Card card("Coal", 1, CardType::FUEL);
        card.setAttribute(AttributeType::WEIGHT, 1.5f);
        card.setAttribute(AttributeType::BURN_VALUE, 30.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 12.0f);
        return card;
    }

    static Card createBandage() {
        Card card("Bandage", 1, CardType::HERB);
        card.setAttribute(AttributeType::WEIGHT, 0.1f);
        card.setAttribute(AttributeType::HEALING, 25.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 10.0f);
        return card;
    }

    static Card createLeatherArmor() {
        Card card("Leather Armor", 2, CardType::ARMOR);
        card.setAttribute(AttributeType::WEIGHT, 2.5f);
        card.setAttribute(AttributeType::DEFENSE, 30.0f);
        card.setAttribute(AttributeType::DURABILITY, 60.0f);
        card.setAttribute(AttributeType::TRADE_VALUE, 35.0f);
        return card;
    }
};

// Random Card Generator
class RandomCardGenerator {
public:
    static Card generateRandomCard() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> cardTypeDist(0, 9);
        
        int cardType = cardTypeDist(gen);
        switch (cardType) {
            case 0: return CardFactory::createWood();
            case 1: return CardFactory::createMetal();
            case 2: return CardFactory::createFood();
            case 3: return CardFactory::createWater();
            case 4: return CardFactory::createMedicine();
            case 5: return CardFactory::createWeapon();
            case 6: return CardFactory::createIronOre();
            case 7: return CardFactory::createCoal();
            case 8: return CardFactory::createBandage();
            case 9: return CardFactory::createLeatherArmor();
            default: return CardFactory::createWood();
        }
    }
    
    static Card generateRandomCardByRarity(int rarity) {
        Card card = generateRandomCard();
        card.rarity = rarity;
        return card;
    }
};

// Initial Game State
inline const std::vector<Card> INITIAL_CARDS = {
    CardFactory::createWood(),
    CardFactory::createMetal(),
    CardFactory::createFood(),
    CardFactory::createWater(),
    CardFactory::createMedicine(),
    CardFactory::createWeapon()
};

// Exploration Events
inline const std::vector<Event> EXPLORATION_EVENTS = {
    Event("Found abandoned warehouse", {CardFactory::createWood(), CardFactory::createMetal()}, {}, 0.25f),
    Event("Zombie attack", {}, {CardFactory::createFood()}, 0.2f),
    Event("Found medical kit", {CardFactory::createMedicine(), CardFactory::createBandage()}, {}, 0.15f),
    Event("Found water source", {CardFactory::createWater()}, {}, 0.15f),
    Event("Mined minerals", {CardFactory::createIronOre(), CardFactory::createCoal()}, {}, 0.1f),
    Event("Encountered merchant", {CardFactory::createLeatherArmor()}, {CardFactory::createMetal()}, 0.1f),
    Event("Safe exploration", {CardFactory::createFood()}, {}, 0.05f)
};
}
