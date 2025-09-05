#!/bin/bash

# FocusableButton Test Script
# This script builds and runs the FocusableButton test window

echo "=== FocusableButton Test Script ==="
echo

# Navigate to project directory
cd "$(dirname "$0")/.."

# Build the project
echo "Building project..."
if make -C build; then
    echo "Build successful!"
else
    echo "Build failed!"
    exit 1
fi

echo
echo "Starting FocusableButton test window..."
echo "Test Features:"
echo "  ✓ Visual states (normal, hover, focus, pressed)"
echo "  ✓ Keyboard navigation (Tab/Shift+Tab)"
echo "  ✓ Keyboard activation (Enter/Space)"
echo "  ✓ Mouse interaction"
echo "  ✓ Focus management"
echo "  ✓ Z-order handling"
echo
echo "Controls:"
echo "  Tab - Focus next button"
echo "  Shift+Tab - Focus previous button"  
echo "  Enter/Space - Activate focused button"
echo "  ESC - Clear focus and exit"
echo "  Mouse - Click on buttons"
echo
echo "Press Enter to start the test window..."
read -r

# Run the test
./build/FocusableButtonTest

echo
echo "FocusableButton test completed!"
echo "Check the terminal output above for focus change logs and button click confirmations."
