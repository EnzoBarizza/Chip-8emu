cmake_minimum_required(VERSION 4.2)
project("CHIP-8")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("src/cpu")
add_subdirectory("src/devices")
add_subdirectory("src/runner")