add_library(interfacelib inteface.c)

target_link_libraries(interfacelib cpu devices)
target_include_directories(interfacelib INTERFACE ${CMAKE_CURRENT_LIST_DIR})