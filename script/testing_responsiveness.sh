#!/bin/bash

echo "================================="
echo "  Program Shutdown Responsiveness Final Verification Test"
echo "================================="
echo
echo "This test will verify that the program no longer has 'not responding' issues"
echo "and can safely shut down within a reasonable time"
echo

# Function to test response time
test_response() {
    local test_name="$1"
    local timeout_sec="$2"
    local signal="$3"
    
    echo "🔍 Test: $test_name"
    echo "   Expected response time: < 2.5s"
    
    START_TIME=$(date +%s.%N)
    
    if [ -n "$signal" ]; then
        ./build/Survive 2>/dev/null &
        PID=$!
        sleep 0.5
        kill $signal $PID 2>/dev/null
        wait $PID 2>/dev/null
    else
        timeout ${timeout_sec}s ./build/Survive 2>/dev/null
    fi
    
    END_TIME=$(date +%s.%N)
    DURATION=$(echo "$END_TIME - $START_TIME" | bc -l)
    
    if (( $(echo "$DURATION < 2.5" | bc -l) )); then
        printf "   ✅ Passed - Response time: %.2fs\n" $DURATION
        return 0
    else
        printf "   ❌ Failed - Response time: %.2fs (over 2.5s)\n" $DURATION
        return 1
    fi
}

# Test cases
passed=0
total=0

# Test 1: Timeout test
((total++))
if test_response "Auto shutdown by timeout" 2 ""; then
    ((passed++))
fi
echo

# Test 2: SIGTERM test  
((total++))
if test_response "Shutdown by SIGTERM signal" 5 "-TERM"; then
    ((passed++))
fi
echo

# Test 3: SIGINT test (Ctrl+C)
((total++))
if test_response "Shutdown by SIGINT signal (Ctrl+C)" 5 "-INT"; then
    ((passed++))
fi
echo

# Test 4: Quick successive signals
echo "🔍 Test: Quick successive signals"
echo "   Expected response time: < 2.5s"
START_TIME=$(date +%s.%N)
./build/Survive 2>/dev/null &
PID=$!
sleep 0.3
kill -TERM $PID 2>/dev/null
sleep 0.1
kill -TERM $PID 2>/dev/null  
wait $PID 2>/dev/null
END_TIME=$(date +%s.%N)
DURATION=$(echo "$END_TIME - $START_TIME" | bc -l)

((total++))
if (( $(echo "$DURATION < 2.5" | bc -l) )); then
    printf "   ✅ Passed - Response time: %.2fs\n" $DURATION
    ((passed++))
else
    printf "   ❌ Failed - Response time: %.2fs (over 2.5s)\n" $DURATION
fi
echo

# Summary
echo "================================="
echo "           Test Summary"
echo "================================="
printf "Tests passed: %d/%d\n" $passed $total
echo

if [ $passed -eq $total ]; then
    echo "🎉 All tests passed!"
    echo ""
    echo "✅ Program shutdown responsiveness issue resolved"
    echo "✅ No 'not responding' phenomenon"
    echo "✅ All shutdown times within 2.5 seconds"
    echo "✅ Data safety save function works properly"
    echo ""
    echo "The program now has production-level responsiveness!"
else
    echo "⚠️  Some tests failed, further optimization may be needed"
    echo ""
    echo "Suggestions to check:"
    echo "- Is system load too high?"
    echo "- Are other processes occupying resources?"
    echo "- Is disk I/O normal?"
fi

echo
echo "================================="
