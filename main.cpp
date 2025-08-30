#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

// RAII wrapper for SDL resources
class SDLManager {
public:
    SDLManager() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
        }
        window = SDL_CreateWindow("Endgame MVP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
        if (!window) {
            throw std::runtime_error("Window creation failed: " + std::string(SDL_GetError()));
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            throw std::runtime_error("Renderer creation failed: " + std::string(SDL_GetError()));
        }
        if (TTF_Init() < 0) {
            throw std::runtime_error("TTF_Init failed: " + std::string(TTF_GetError()));
        }
        font = TTF_OpenFont("./assets/font.ttf", 16);
        if (!font) {
            throw std::runtime_error("Font loading failed: " + std::string(TTF_GetError()));
        }
    }

    ~SDLManager() {
        if (font) TTF_CloseFont(font);
        TTF_Quit();
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }

    SDL_Window* getWindow() const { return window; }
    SDL_Renderer* getRenderer() const { return renderer; }
    TTF_Font* getFont() const { return font; }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
};

// Card structure
struct Card {
    std::string name;
    int rarity; // 1=Common, 2=Rare, 3=Legendary
    int quantity; // Quantity (used for merging)
    Card(const std::string& n, int r, int q = 1) : name(n), rarity(r), quantity(q) {}
};

// Inventory system
class Inventory {
public:
    void addCard(const Card& card) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& c : cards) {
            if (c.name == card.name && c.rarity == card.rarity) {
                c.quantity += card.quantity;
                return;
            }
        }
        cards.push_back(card);
    }

    void removeCard(const std::string& name, int rarity) {
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

    void updateCards(const std::vector<Card>& newCards) {
        std::lock_guard<std::mutex> lock(mutex);
        cards = newCards;
    }

    const std::vector<Card>& getCards() const { return cards; }

private:
    std::vector<Card> cards;
    std::mutex mutex;
};

// View: rendering logic
class View {
public:
    View(SDLManager& sdl) : sdlManager(sdl) {}

    void render(const Inventory& inventory, const Card* selectedCard, int mouseX, int mouseY) {
        SDL_Renderer* renderer = sdlManager.getRenderer();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render cards in the inventory
        int index = 0;
        for (const auto& card : inventory.getCards()) {
            int cardY = 50 + index * 60;
            SDL_Rect rect = {50, cardY, 200, 50};
            setRarityColor(card.rarity);
            SDL_RenderFillRect(renderer, &rect);
            std::string text = card.name + " x" + std::to_string(card.quantity);
            renderText(text, 55, cardY + 15, {255, 255, 255, 255});
            index++;
        }

        // Render the card being dragged
        if (selectedCard) {
            SDL_Rect dragRect = {mouseX - 100, mouseY - 25, 200, 50};
            setRarityColor(selectedCard->rarity);
            SDL_RenderFillRect(renderer, &dragRect);
            std::string text = selectedCard->name + " x" + std::to_string(selectedCard->quantity);
            renderText(text, mouseX - 95, mouseY - 10, {255, 255, 255, 255});
        }

        // Render buttons
        renderButton("Add Card", 600, 50, 100, 40);
        renderButton("Remove Card", 600, 100, 100, 40);

        SDL_RenderPresent(renderer);
    }

private:
    SDLManager& sdlManager;

    void setRarityColor(int rarity) {
        SDL_Renderer* renderer = sdlManager.getRenderer();
        switch (rarity) {
            case 1: SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break;
            case 2: SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255); break;
            case 3: SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); break;
        }
    }

    void renderText(const std::string& text, int x, int y, SDL_Color color) {
        SDL_Surface* surface = TTF_RenderText_Solid(sdlManager.getFont(), text.c_str(), color);
        if (!surface) {
            std::cerr << "Text rendering failed: " << TTF_GetError() << std::endl;
            return;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlManager.getRenderer(), surface);
        if (!texture) {
            std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surface);
            return;
        }
        int textW, textH;
        TTF_SizeUTF8(sdlManager.getFont(), text.c_str(), &textW, &textH);
        SDL_Rect dst = {x, y, textW, textH};
        SDL_RenderCopy(sdlManager.getRenderer(), texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    void renderButton(const std::string& text, int x, int y, int minW, int h) {
        SDL_Renderer* renderer = sdlManager.getRenderer();
        int textW, textH;
        TTF_SizeUTF8(sdlManager.getFont(), text.c_str(), &textW, &textH);
        int buttonW = std::max(minW, textW + 10); // Dynamically adjust button width
        SDL_Rect rect = {x, y, buttonW, h};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Gray button
        SDL_RenderFillRect(renderer, &rect);
        int textX = x + (buttonW - textW) / 2; // Center the text
        int textY = y + (h - textH) / 2;
        renderText(text, textX, textY, {255, 255, 255, 255});
    }
};

// Controller: event handling and game logic
class Controller {
public:
    Controller(Inventory& inv, View& v) : inventory(inv), view(v), selectedCard(nullptr), running(true) {}

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        handleMouseDown(event.button.x, event.button.y);
                    }
                    break;
                case SDL_MOUSEMOTION:
                    mouseX = event.motion.x;
                    mouseY = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        selectedCard = nullptr;
                    }
                    break;
            }
        }
    }

    bool isRunning() const { return running; }

    void updateView() {
        view.render(inventory, selectedCard, mouseX, mouseY);
    }

    void organizeInventory() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<std::string> cardNames = {"wood", "metal", "food"};
        std::uniform_int_distribution<> rarityDist(1, 3);
        std::uniform_int_distribution<> nameDist(0, 2);

        while (running) {
            {
                std::lock_guard<std::mutex> lock(mutex);
                std::vector<Card> newCards;
                for (const auto& card : inventory.getCards()) {
                    bool merged = false;
                    for (auto& newCard : newCards) {
                        if (newCard.name == card.name && newCard.rarity == card.rarity) {
                            newCard.quantity += card.quantity;
                            merged = true;
                            break;
                        }
                    }
                    if (!merged) newCards.push_back(card);
                }
                inventory.updateCards(newCards);
                // Simulate random card addition
                inventory.addCard(Card(cardNames[nameDist(gen)], rarityDist(gen)));
            }
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

private:
    Inventory& inventory;
    View& view;
    Card* selectedCard;
    int mouseX = 0, mouseY = 0;
    bool running;
    std::mutex mutex;

    void handleMouseDown(int x, int y) {
        // Check button clicks
        if (x >= 600 && x <= 700 && y >= 50 && y <= 90) {
            // Add Card button
            std::random_device rd;
            std::mt19937 gen(rd());
            std::vector<std::string> cardNames = {"wood", "metal", "food"};
            std::uniform_int_distribution<> rarityDist(1, 3);
            std::uniform_int_distribution<> nameDist(0, 2);
            inventory.addCard(Card(cardNames[nameDist(gen)], rarityDist(gen)));
        } else if (x >= 600 && x <= 700 && y >= 100 && y <= 140) {
            // Remove Card button
            auto& cards = inventory.getCards();
            if (!cards.empty()) {
                inventory.removeCard(cards[0].name, cards[0].rarity);
            }
        }

        // Check card selection
        int index = 0;
        for (const auto& card : inventory.getCards()) {
            int cardY = 50 + index * 60;
            if (x >= 50 && x <= 250 && y >= cardY && y <= cardY + 50) {
                selectedCard = const_cast<Card*>(&card);
                return;
            }
            index++;
        }
        selectedCard = nullptr;
    }
};

// Game class
class Game {
public:
    Game() : sdlManager(), inventory(), view(sdlManager), controller(inventory, view) {
        inventory.addCard(Card("Wood", 1));
        inventory.addCard(Card("Metal", 2));
        inventory.addCard(Card("Food", 1, 2));
    }

    void run() {
        std::thread organizer([this]() { controller.organizeInventory(); });
        while (controller.isRunning()) {
            controller.handleEvents();
            controller.updateView();
            SDL_Delay(16);
        }
        if (organizer.joinable()) organizer.join();
    }

private:
    SDLManager sdlManager;
    Inventory inventory;
    View view;
    Controller controller;
};

int main(int argc, char* argv[]) {
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}