add_library(cpu_lib cpu.c instructions.c)
target_include_directories(cpu_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})