#include "Game.h"
#include "Constants.h"
#include <thread>

Game::Game() : sdlManager(), inventory(), view(sdlManager), controller(inventory, view) {
    for (const auto& card : Constants::INITIAL_CARDS) {
        inventory.addCard(card);
    }
}

void Game::run() {
    std::thread organizer([this]() { controller.organizeInventory(); });
    while (controller.isRunning()) {
        controller.handleEvents();
        controller.updateView();
        SDL_Delay(Constants::FRAME_DELAY_MS);
    }
    if (organizer.joinable()) organizer.join();
}