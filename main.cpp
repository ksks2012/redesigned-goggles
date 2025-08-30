#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

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
    Inventory() = default;

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

class Game {
private:
    SDLManager sdlManager;
    Inventory inventory;
    bool running;
    Card* selectedCard;
    int mouseX, mouseY;
    std::mutex mutex; // Added mutex for Game class to protect inventory access

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        selectCard(event.button.x, event.button.y);
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

    void selectCard(int x, int y) {
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

        void render() {
        SDL_Renderer* renderer = sdlManager.getRenderer();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render cards in the inventory
        int index = 0;
        for (const auto& card : inventory.getCards()) {
            int cardY = 50 + index * 60;
            SDL_Rect rect = {50, cardY, 200, 50};
            // Set color based on rarity
            switch (card.rarity) {
                case 1: SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break; // Common
                case 2: SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255); break; // Rare
                case 3: SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); break;  // Legendary
            }
            SDL_RenderFillRect(renderer, &rect);

            // Render card name and quantity
            std::string text = card.name + " x" + std::to_string(card.quantity);
            SDL_Color color = {255, 255, 255, 255};
            renderText(text, 55, cardY + 15, color);
            index++;
        }

        // Render the card being dragged
        if (selectedCard) {
            SDL_Rect dragRect = {mouseX - 100, mouseY - 25, 200, 50};
            switch (selectedCard->rarity) {
                case 1: SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break;
                case 2: SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255); break;
                case 3: SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); break;
            }
            SDL_RenderFillRect(renderer, &dragRect);
            std::string text = selectedCard->name + " x" + std::to_string(selectedCard->quantity);
            renderText(text, mouseX - 95, mouseY - 10, {255, 255, 255, 255});
        }

        SDL_RenderPresent(renderer);
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
        SDL_Rect dst = {x, y, surface->w, surface->h};
        SDL_RenderCopy(sdlManager.getRenderer(), texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
    
    void organizeInventory() {
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
            }
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

public:
    Game() : sdlManager(), inventory(), running(true), selectedCard(nullptr) {
        inventory.addCard(Card("Wood", 1));
        inventory.addCard(Card("Metal", 2));
        inventory.addCard(Card("Food", 1, 2));
    }

    void run() {
        std::thread organizer([this]() { organizeInventory(); });
        while (running) {
            handleEvents();
            render();
            SDL_Delay(16);
        }
        running = false;
        if (organizer.joinable()) organizer.join();
    }
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