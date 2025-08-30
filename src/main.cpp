#include "Game.h"
#include <stdexcept>
#include <iostream>

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