add_executable(main main.c)
target_include_directories(main PUBLIC ${CHIP8_HEADERS})
target_link_libraries(main chip8)