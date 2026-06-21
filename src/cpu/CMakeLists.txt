add_library(cpu_lib cpu.c)
target_include_directories(cpu_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})