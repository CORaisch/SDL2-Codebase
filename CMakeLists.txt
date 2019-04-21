cmake_minimum_required (VERSION 3.5)
project (SDL2_Test)

set (CMAKE_CXX_STANDARD 11)

find_package (SDL2 REQUIRED)
include_directories (${SDL_INCLUDE_DIRS})

add_executable (sdl2_window src/sdl2_window.cpp)
target_link_libraries (sdl2_window SDL2::SDL2)
