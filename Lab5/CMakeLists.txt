cmake_minimum_required(VERSION 3.25)
project(Lab5 C)

set(CMAKE_C_STANDARD 11)

add_executable(Lab5 main.c)
target_link_libraries(Lab5 gdi32)