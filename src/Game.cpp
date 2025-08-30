#include "Game.h"
#include <thread>

Game::Game() : sdlManager(), inventory(), view(sdlManager), controller(inventory, view) {
    inventory.addCard(Card("Wood", 1));
    inventory.addCard(Card("Metal", 2));
    inventory.addCard(Card("Food", 1, 2));
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