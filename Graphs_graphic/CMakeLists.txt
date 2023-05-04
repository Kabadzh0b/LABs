cmake_minimum_required(VERSION 3.25)
project(Graphs_graphic C)

set(CMAKE_C_STANDARD 11)

add_executable(Graphs_graphic main.c)
target_link_libraries(Graphs_graphic gdi32)