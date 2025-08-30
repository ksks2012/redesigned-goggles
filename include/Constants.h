#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <chrono>
#include "Card.h"
#include "Event.h"

namespace Constants {
// SDLManager
constexpr const char* WINDOW_TITLE = "Endgame MVP";
constexpr int WINDOW_POS = SDL_WINDOWPOS_CENTERED;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
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

// Game: Initial cards
inline const std::vector<Card> INITIAL_CARDS = {
    Card("Wood", 1),
    Card("Metal", 2),
    Card("Food", 1, 2),
    Card("Water", 1),
    Card("Medicine", 2),
    Card("Weapon", 3)
};

// Event: Exploration events
inline const std::vector<Event> EXPLORATION_EVENTS = {
    Event("Found abandoned warehouse", {Card("Wood", 1, 3)}, {}, 0.4f),
    Event("Zombie attack", {}, {Card("Food", 1, 1)}, 0.3f),
    Event("Found medical kit", {Card("Medicine", 2, 1)}, {}, 0.2f),
    Event("Found water source", {Card("Water", 1, 2)}, {}, 0.1f)
};
}