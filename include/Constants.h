#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include "Card.h"
#include "Event.h"

namespace Constants {
// SDLManager
constexpr const char* WINDOW_TITLE = "Endgame MVP";
constexpr int WINDOW_POS = SDL_WINDOWPOS_CENTERED;
constexpr int WINDOW_WIDTH = 1080;
constexpr int WINDOW_HEIGHT = 720;
constexpr Uint32 WINDOW_FLAGS = SDL_WINDOW_SHOWN;
constexpr Uint32 RENDERER_FLAGS = SDL_RENDERER_ACCELERATED;
constexpr const char* FONT_PATH = "./assets/font.ttf";
constexpr int FONT_SIZE = 16;

// View: Colors
constexpr SDL_Color BACKGROUND_COLOR = {0, 0, 0, 255}; // Black
constexpr SDL_Color BUTTON_COLOR = {50, 50, 50, 255}; // Gray
constexpr SDL_Color TEXT_COLOR = {255, 255, 255, 255}; // White
constexpr SDL_Color RARITY_COMMON = {100, 100, 100, 255};
constexpr SDL_Color RARITY_RARE = {100, 100, 255, 255};
constexpr SDL_Color RARITY_LEGENDARY = {255, 215, 0, 255};
constexpr SDL_Color ATTRIBUTE_TEXT_COLOR = {200, 200, 200, 255}; // Light gray for attributes
constexpr SDL_Color TOOLTIP_BG_COLOR = {30, 30, 30, 230}; // Dark gray with some transparency


// View: Card rendering
constexpr int CARD_X = 50;
constexpr int CARD_WIDTH = 200;
constexpr int CARD_HEIGHT = 50;
constexpr int CARD_SPACING = 60;
constexpr int CARD_TEXT_OFFSET_X = 5;
constexpr int CARD_TEXT_OFFSET_Y = 15;
constexpr int DRAG_CARD_OFFSET_X = -100;
constexpr int DRAG_CARD_OFFSET_Y = -25;
constexpr int DRAG_TEXT_OFFSET_X = -95;
constexpr int DRAG_TEXT_OFFSET_Y = -10;

// View: Button rendering
constexpr int BUTTON_X = 600;
constexpr int BUTTON_MIN_WIDTH = 100;
constexpr int BUTTON_HEIGHT = 40;
constexpr int BUTTON_Y_ADD = 50;
constexpr int BUTTON_Y_REMOVE = 100;
constexpr int BUTTON_Y_EXPLORE = 150;
constexpr const char* BUTTON_TEXT_ADD = "Add Card";
constexpr const char* BUTTON_TEXT_REMOVE = "Remove Card";
constexpr const char* BUTTON_TEXT_EXPLORE = "Explore";

// Controller: Hitboxes
constexpr int BUTTON_MAX_X = BUTTON_X + BUTTON_MIN_WIDTH;
constexpr int BUTTON_Y_ADD_END = BUTTON_Y_ADD + BUTTON_HEIGHT;
constexpr int BUTTON_Y_REMOVE_END = BUTTON_Y_REMOVE + BUTTON_HEIGHT;
constexpr int BUTTON_Y_EXPLORE_END = BUTTON_Y_EXPLORE + BUTTON_HEIGHT;
constexpr int CARD_MAX_X = CARD_X + CARD_WIDTH;

// Controller: Game logic
constexpr std::chrono::seconds ORGANIZE_INTERVAL{10};
constexpr int FRAME_DELAY_MS = 16;
constexpr int RARITY_MIN = 1;
constexpr int RARITY_MAX = 3;
inline const std::vector<std::string> CARD_NAMES = {"Wood", "Metal", "Food", "Water", "Medicine", "Weapon"};

// Card factory functions - Create default cards with attributes

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

// Game: Initial cards (using the new CardFactory)
inline const std::vector<Card> INITIAL_CARDS = {
    CardFactory::createWood(),
    CardFactory::createMetal(),
    CardFactory::createFood(),
    CardFactory::createWater(),
    CardFactory::createMedicine(),
    CardFactory::createWeapon()
};

// Event: Exploration events (using the new CardFactory)
inline const std::vector<Event> EXPLORATION_EVENTS = {
    Event("Found abandoned warehouse", {CardFactory::createWood(), CardFactory::createMetal()}, {}, 0.25f),
    Event("Zombie attack", {}, {CardFactory::createFood()}, 0.2f),
    Event("Found medical kit", {CardFactory::createMedicine(), CardFactory::createBandage()}, {}, 0.15f),
    Event("Found water source", {CardFactory::createWater()}, {}, 0.15f),
    Event("Mined minerals", {CardFactory::createIronOre(), CardFactory::createCoal()}, {}, 0.1f),
    Event("Encountered merchant", {CardFactory::createLeatherArmor()}, {CardFactory::createMetal()}, 0.1f),
    Event("Safe exploration", {CardFactory::createFood()}, {}, 0.05f)
};

// Tooltip and hints
constexpr int HINT_X = 20;
constexpr int HINT_BOTTOM_OFFSET = 120;
constexpr int HINT_LINE_SPACING = 28;
constexpr const char* HINT_TITLE = "Hints:";
constexpr const char* HINT_SAVE = "S: Save progress";
constexpr const char* HINT_LOAD = "L: Load progress";
constexpr const char* HINT_EXIT = "ESC: Exit game";

// Tooltip text
const std::string TOOLTIP_RARITY = "Rarity";
const std::string TOOLTIP_COMMON = "Common";
const std::string TOOLTIP_RARE = "Rare";
const std::string TOOLTIP_LEGENDARY = "Legendary";
const std::string TOOLTIP_QUANTITY = "Quantity";
const std::string TOOLTIP_WEIGHT = "Weight";
const std::string TOOLTIP_TOTAL_WEIGHT = "Total Weight";
const std::string TOOLTIP_NUTRITION = "Nutrition";
const std::string TOOLTIP_ATTACK = "Attack";
const std::string TOOLTIP_DEFENSE = "Defense";
const std::string TOOLTIP_HEALING = "Healing";
const std::string TOOLTIP_DURABILITY = "Durability";
const std::string TOOLTIP_BURN_VALUE = "Burn Value";
const std::string TOOLTIP_CRAFTING_VALUE = "Crafting Value";
const std::string TOOLTIP_TRADE_VALUE = "Trade Value";
const std::string TOOLTIP_EDIBLE = "Edible";
const std::string TOOLTIP_BURNABLE = "Burnable";
}