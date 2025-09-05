#include "Interface/ui/FocusableButtonTestWindow.h"
#include <iostream>

/**
 * Simple test program for FocusableButton functionality
 * 
 * This program creates a window with multiple FocusableButton instances
 * to demonstrate and test:
 * - Visual states (normal, hovered, focused, pressed)
 * - Keyboard navigation (Tab/Shift+Tab)
 * - Keyboard activation (Enter/Space)
 * - Mouse interaction
 * - Focus management
 * - Z-order handling
 */
int main() {
    std::cout << "Starting FocusableButton test..." << std::endl;
    
    FocusableButtonTestWindow testWindow;
    
    if (!testWindow.initialize()) {
        std::cerr << "Failed to initialize test window!" << std::endl;
        return -1;
    }
    
    // Run the test window
    testWindow.run();
    
    std::cout << "FocusableButton test completed." << std::endl;
    return 0;
}
