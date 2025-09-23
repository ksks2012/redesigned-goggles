#!/bin/bash

echo "🧪 Testing Tech Tree Progress Bars..."
echo "This script will start the TechTreeUITest and simulate key presses"

cd /home/hong/code/C++/survive/build

# Create a simple input sequence:
# R = Add research points (100)
# R = Add more research points (200 total) 
# Enter = Start research on selected tech (Basic Survival)
# Wait a few seconds for progress to show
# Q = Quit

echo "Press sequence: R (add points) -> R (more points) -> Enter (start research) -> Q (quit after 3 sec)"

(
  sleep 1
  echo "r"    # Add research points
  sleep 0.5
  echo "r"    # Add more research points  
  sleep 0.5
  echo ""     # Press Enter to start research
  sleep 3     # Wait to see progress animation
  echo "q"    # Quit
) | timeout 10 ./TechTreeUITest

echo "✅ Test completed!"
