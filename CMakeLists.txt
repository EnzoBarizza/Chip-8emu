cmake_minimum_required(VERSION 4.2)
project("CHIP-8")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
)
FetchContent_MakeAvailable(raylib)

add_executable(chip8 src/cpu.c src/main.c)
target_link_libraries(chip8 PRIVATE raylib)