cmake_minimum_required(VERSION 4.2)
project("Chip-8emu")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("src/chip8")
add_subdirectory("src/run")