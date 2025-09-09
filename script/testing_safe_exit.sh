#!/bin/bash

echo "=== Test Program Safe Exit Function ==="
echo "Starting the program and will close automatically after 3 seconds..."

# Start the program and send SIGTERM signal after 3 seconds
timeout 3s ./build/Survive &
GAME_PID=$!

echo "Program PID: $GAME_PID"
echo "Waiting 3 seconds before sending exit signal..."

# Wait for the program to exit naturally or be terminated by timeout
wait $GAME_PID
EXIT_CODE=$?

echo ""
if [ $EXIT_CODE -eq 124 ]; then
    echo "✓ Program was correctly terminated by timeout (exit code: $EXIT_CODE)"
elif [ $EXIT_CODE -eq 0 ]; then
    echo "✓ Program exited normally (exit code: $EXIT_CODE)"
else
    echo "✗ Program exited abnormally (exit code: $EXIT_CODE)"
fi

echo ""
echo "Test completed!"

# Test window close event again
echo ""
echo "=== Test SDL_QUIT Event Handling ==="
echo "Start the program and immediately send SIGTERM..."

timeout 1s ./build/Survive &
GAME_PID2=$!

sleep 0.5
kill -TERM $GAME_PID2 2>/dev/null

wait $GAME_PID2 2>/dev/null
EXIT_CODE2=$?

echo "Second test exit code: $EXIT_CODE2"

if [ $EXIT_CODE2 -eq 143 ] || [ $EXIT_CODE2 -eq 0 ] || [ $EXIT_CODE2 -eq 124 ]; then
    echo "✓ Program correctly responds to termination signal"
else
    echo "✗ Program response to termination signal is abnormal"
fi

echo ""
echo "All tests completed!"
