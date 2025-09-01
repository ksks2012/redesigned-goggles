#!/bin/bash

echo "=== Game Editor Feature Demo ==="
echo "Starting the game and simulating F1 key press (automatically entering editor mode)..."

# Use timeout and input pipe to simulate editor operations
timeout 10s bash -c '
# Simulate editor command sequence
echo -e "\nhelp\nmaterial\n1\ndata\n1\nquit" | ./build/Survive 2>/dev/null
'

echo ""
echo "Demo complete!"
echo ""
echo "For actual use:"
echo "1. Run ./build/Survive"
echo "2. Press the F1 key in the game window"
echo "3. Enter editor commands in the console"
echo "4. You can perform the following operations:"
echo "   - material: Material editing"
echo "   - recipe: Recipe editing"
echo "   - event: Event editing"
echo "   - data: Data management"
echo "   - help: View help"
echo "   - quit: Exit editor"