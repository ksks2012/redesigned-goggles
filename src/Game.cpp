#include "Game.h"
#include <thread>

Game::Game() : sdlManager(), inventory(), view(sdlManager), controller(inventory, view) {
        inventory.addCard(Card("Wood", 1));
        inventory.addCard(Card("Metal", 2));
        inventory.addCard(Card("Food", 1, 2));
        inventory.addCard(Card("Water", 1));
        inventory.addCard(Card("Medicine", 2));
        inventory.addCard(Card("Weapon", 3));
    }

void Game::run() {
    std::thread organizer([this]() { controller.organizeInventory(); });
    while (controller.isRunning()) {
        controller.handleEvents();
        controller.updateView();
        SDL_Delay(16);
    }
    if (organizer.joinable()) organizer.join();
}