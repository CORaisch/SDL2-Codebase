cmake_minimum_required (VERSION 3.5)
project (SDL2_Test)

set (CMAKE_CXX_STANDARD 11)

find_package (SDL2 REQUIRED)
include_directories (${SDL_INCLUDE_DIRS} include)

add_executable (sdl2_window src/sdl2_window.cpp)
target_link_libraries (sdl2_window SDL2::SDL2)

add_executable (sdl2_synthesizer src/sdl2_synthesizer.cpp src/sdl2_eventhandler.cpp)
target_link_libraries (sdl2_synthesizer SDL2::SDL2)

add_executable (sdl2_eventhandler src/sdl2_test_eventhandler.cpp src/sdl2_eventhandler.cpp)
target_link_libraries (sdl2_eventhandler SDL2::SDL2)

add_executable (sdl2_crazy src/sdl2_crazy.cpp)
target_link_libraries (sdl2_crazy SDL2::SDL2)
