#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <csignal>
#include <memory>

namespace GameSystem {

// Forward declaration
class SimpleGameController;

/**
 * Signal handler for graceful shutdown
 * Handles SIGTERM, SIGINT to allow proper cleanup
 */
class SignalHandler {
public:
    static void setup(std::shared_ptr<SimpleGameController> gameController);
    static void cleanup();
    
private:
    static void signalCallback(int signal);
    static std::weak_ptr<SimpleGameController> gameController_;
};

} // namespace GameSystem

#endif // SIGNAL_HANDLER_H
