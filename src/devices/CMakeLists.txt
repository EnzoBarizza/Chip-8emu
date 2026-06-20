add_library(devices_lib display.c)
target_include_directories(devices_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})

set(BUILD_SHARED_LIBS OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG origin/main
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(SDL3)

target_link_libraries(devices_lib PUBLIC SDL3::SDL3-static)