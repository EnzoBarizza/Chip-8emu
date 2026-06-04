add_library(chip8 chip8.c instructions.c)
target_include_directories(chip8 INTERFACE ${CMAKE_CURRENT_LIST_DIR})