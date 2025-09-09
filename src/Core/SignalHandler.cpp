#include "Core/SignalHandler.h"
#include "Core/SimpleGameController.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

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
        // First, stop the main game loop
        controller->stop();
        
        // Then force stop background processes for immediate shutdown
        // This bypasses the normal organize inventory cycle
        std::cout << "Forcing immediate shutdown..." << std::endl;
    }
    
    // For critical signals, set a flag to force immediate exit
    if (signal == SIGTERM || signal == SIGINT) {
        // Reduce force exit timeout to 1 second for ultra-fast response
        std::thread([signal]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Force exit due to signal " << signal << std::endl;
            std::_Exit(0); // Use _Exit for immediate termination
        }).detach();
    }
}

} // namespace GameSystem
