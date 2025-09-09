#include "Core/SignalHandler.h"
#include "Core/SimpleGameController.h"
#include <iostream>

namespace GameSystem {

std::weak_ptr<SimpleGameController> SignalHandler::gameController_;

void SignalHandler::setup(std::shared_ptr<SimpleGameController> gameController) {
    gameController_ = gameController;
    
    // Register signal handlers
    std::signal(SIGTERM, signalCallback);
    std::signal(SIGINT, signalCallback);
    
    std::cout << "Signal handlers registered for graceful shutdown" << std::endl;
}

void SignalHandler::cleanup() {
    // Reset signal handlers to default
    std::signal(SIGTERM, SIG_DFL);
    std::signal(SIGINT, SIG_DFL);
    
    gameController_.reset();
}

void SignalHandler::signalCallback(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down gracefully..." << std::endl;
    
    if (auto controller = gameController_.lock()) {
        controller->stop();
    }
}

} // namespace GameSystem
