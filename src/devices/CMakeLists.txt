add_library(devices_lib display.c)
target_include_directories(devices_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG origin/master
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(raylib)

target_link_libraries(devices_lib PUBLIC raylib)