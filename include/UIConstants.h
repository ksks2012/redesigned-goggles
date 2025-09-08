#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace UIConstants {
// SDLManager Window Settings
constexpr const char* WINDOW_TITLE = "Endgame MVP";
constexpr int WINDOW_POS = SDL_WINDOWPOS_CENTERED;
constexpr int WINDOW_WIDTH = 1080;
constexpr int WINDOW_HEIGHT = 720;
constexpr Uint32 WINDOW_FLAGS = SDL_WINDOW_SHOWN;
constexpr Uint32 RENDERER_FLAGS = SDL_RENDERER_ACCELERATED;
constexpr const char* FONT_PATH = "./assets/font.ttf";
constexpr int FONT_SIZE = 16;

// UI Colors - Basic
constexpr SDL_Color BACKGROUND_COLOR = {0, 0, 0, 255}; // Black
constexpr SDL_Color BUTTON_COLOR = {50, 50, 50, 255}; // Gray
constexpr SDL_Color TEXT_COLOR = {255, 255, 255, 255}; // White
constexpr SDL_Color ATTRIBUTE_TEXT_COLOR = {200, 200, 200, 255}; // Light gray for attributes
constexpr SDL_Color TOOLTIP_BG_COLOR = {30, 30, 30, 230}; // Dark gray with some transparency

// UI Colors - Layout Elements
constexpr SDL_Color OVERLAY_COLOR = {0, 0, 0, 128}; // Semi-transparent black overlay
constexpr SDL_Color PANEL_BG_COLOR = {40, 40, 40, 255}; // Dark gray panel background
constexpr SDL_Color BORDER_COLOR = {100, 100, 100, 255}; // Gray border
constexpr SDL_Color SECONDARY_TEXT_COLOR = {180, 180, 180, 255}; // Light gray text
constexpr SDL_Color SELECTED_BORDER_COLOR = {255, 0, 0, 255}; // Red border for selected items

// Rarity Colors
constexpr SDL_Color RARITY_COMMON = {100, 100, 100, 255};
constexpr SDL_Color RARITY_RARE = {100, 100, 255, 255};
constexpr SDL_Color RARITY_LEGENDARY = {255, 215, 0, 255};

// Recipe Item Colors
constexpr SDL_Color RECIPE_CAN_CRAFT_BG = {20, 60, 20, 255}; // Green background for craftable items
constexpr SDL_Color RECIPE_CANNOT_CRAFT_BG = {60, 20, 20, 255}; // Red background for non-craftable items
constexpr SDL_Color RECIPE_LOCKED_BG = {30, 30, 30, 255}; // Dark gray for locked recipes
constexpr SDL_Color RECIPE_DISABLED_TEXT = {150, 150, 150, 255}; // Gray text for disabled items

// Focusable Button Colors
constexpr SDL_Color NORMAL_COLOR = {100, 100, 100, 255};
constexpr SDL_Color HOVERED_COLOR = {120, 120, 120, 255};
constexpr SDL_Color FOCUSED_COLOR = {80, 120, 180, 255};
constexpr SDL_Color PRESSED_COLOR = {60, 60, 60, 255};
constexpr SDL_Color FOCUS_BORDER_COLOR = {255, 255, 0, 255};

// Card Rendering Layout
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

// Inventory Area Layout
constexpr int INVENTORY_AREA_X = 50;
constexpr int INVENTORY_AREA_Y = 50;
constexpr int INVENTORY_AREA_WIDTH = 250;
constexpr int INVENTORY_AREA_HEIGHT = 400;
constexpr int INVENTORY_MARGIN = 10;

// Button Layout
constexpr int BUTTON_X = 900;
constexpr int BUTTON_MIN_WIDTH = 100;
constexpr int BUTTON_HEIGHT = 40;
constexpr int BUTTON_Y_ADD = 50;
constexpr int BUTTON_Y_REMOVE = 100;
constexpr int BUTTON_Y_EXPLORE = 150;
constexpr int BUTTON_Y_CRAFT = 200;
constexpr const char* BUTTON_TEXT_ADD = "Add Card";
constexpr const char* BUTTON_TEXT_REMOVE = "Remove Card";
constexpr const char* BUTTON_TEXT_EXPLORE = "Explore";
constexpr const char* BUTTON_TEXT_CRAFT = "Crafting";
constexpr int BUTTON_TEXT_PADDING = 10;

// Crafting UI Layout
constexpr int CRAFT_PANEL_X = 300;
constexpr int CRAFT_PANEL_Y = 100;
constexpr int CRAFT_PANEL_WIDTH = 450;
constexpr int CRAFT_PANEL_HEIGHT = 400;
constexpr int RECIPE_LIST_HEIGHT = 300;
constexpr int RECIPE_ITEM_HEIGHT = 60;
constexpr int RECIPE_SCROLL_SPEED = 3;

// Crafting Panel Detailed Layout
constexpr int CRAFT_PANEL_PADDING = 20;
constexpr int CRAFT_TITLE_OFFSET_Y = 20;
constexpr int CRAFT_CLOSE_HINT_OFFSET_Y = 45;
constexpr int CRAFT_RECIPE_LIST_START_Y = 80;
constexpr int CRAFT_RECIPE_MARGIN = 10;
constexpr int CRAFT_RECIPE_SPACING = 5;
constexpr int CRAFT_INGREDIENT_OFFSET_X = 250;
constexpr int CRAFT_RESULT_OFFSET_Y = 35;
constexpr int CRAFT_SUCCESS_RATE_OFFSET_Y = 25;
constexpr int CRAFT_PANEL_TITLE_OFFSET_X = 20;
constexpr int CRAFT_PANEL_TITLE_OFFSET_Y = 20;
constexpr int CRAFT_PANEL_HINT_OFFSET_Y = 45;
constexpr int CRAFT_PANEL_RECIPES_START_Y = 80;
constexpr int CRAFT_PANEL_MARGIN = 10;
constexpr int CRAFT_PANEL_BOTTOM_MARGIN = 20;
constexpr int CRAFT_PANEL_RECIPE_LIST_OFFSET = 50; // Offset for calculating recipe selection
constexpr int RECIPE_ITEM_MARGIN = 20; // Space to leave on right side of recipe items
constexpr int RECIPE_ITEM_VERTICAL_SPACING = 5; // Vertical spacing between recipe items

// Tooltip Layout
constexpr int TOOLTIP_LINE_HEIGHT = 30;
constexpr int TOOLTIP_PADDING = 10;
constexpr int TOOLTIP_MOUSE_OFFSET = 15;
constexpr int TOOLTIP_SCREEN_MARGIN = 10;

// Base Building Area Layout
constexpr int BASE_AREA_START_X = 400;    // Base area starts at x = 400
constexpr int BASE_AREA_END_X = 800;      // Base area ends at x = 800  
constexpr int BASE_AREA_START_Y = 200;    // Base area starts at y = 200
constexpr int BASE_AREA_END_Y = 600;      // Base area ends at y = 600
constexpr int GRID_CELL_WIDTH = 80;       // Each grid cell is 80 pixels wide
constexpr int GRID_CELL_HEIGHT = 80;      // Each grid cell is 80 pixels high

// Hints and Help Text
constexpr int HINT_X = WINDOW_WIDTH - 200;
constexpr int HINT_BOTTOM_OFFSET = 120;
constexpr int HINT_LINE_SPACING = 28;
constexpr const char* HINT_TITLE = "Hints:";
constexpr const char* HINT_SAVE = "S: Save progress";
constexpr const char* HINT_LOAD = "L: Load progress";
constexpr const char* HINT_EXIT = "ESC: Exit game";

// Tooltip Text Content
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

// Input and Interaction
constexpr int CLICK_INTERVAL_MS = 100; // Milliseconds
constexpr int DRAG_DROP_TOLERANCE = 10; // Pixels tolerance for drop validation

// UI Hitboxes (calculated constants)
constexpr int BUTTON_MAX_X = BUTTON_X + BUTTON_MIN_WIDTH;
constexpr int BUTTON_Y_ADD_END = BUTTON_Y_ADD + BUTTON_HEIGHT;
constexpr int BUTTON_Y_REMOVE_END = BUTTON_Y_REMOVE + BUTTON_HEIGHT;
constexpr int BUTTON_Y_EXPLORE_END = BUTTON_Y_EXPLORE + BUTTON_HEIGHT;
constexpr int BUTTON_Y_CRAFT_END = BUTTON_Y_CRAFT + BUTTON_HEIGHT;
constexpr int CARD_MAX_X = CARD_X + CARD_WIDTH;
}
