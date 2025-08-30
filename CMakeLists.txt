cmake_minimum_required(VERSION 3.10)
project(Survive)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(Survive main.cpp)
target_link_libraries(Survive SDL2::SDL2 SDL2_ttf::SDL2_ttf)