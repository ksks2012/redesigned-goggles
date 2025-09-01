#!/bin/bash

cd "$(git rev-parse --show-toplevel)"

g++ -std=c++17 -I./include -I./lib ./testing/testing_view_refactor.cpp -o testing_view_refactor -L./build -lSurviveLib -lSDL2 -lSDL2_ttf && ./testing_view_refactor

g++ -std=c++17 -I./include -I./lib ./testing/testing_card_detection.cpp -o testing_card_detection -L./build -lSurviveLib -lSDL2 -lSDL2_ttf && ./testing_card_detection

g++ -std=c++17 -I./include -I./lib ./testing/testing_drag_functionality.cpp -o testing_drag_functionality -L./build -lSurviveLib -lSDL2 -lSDL2_ttf && ./testing_drag_functionality

# Cleanup
rm testing_view_refactor testing_card_detection testing_drag_functionality