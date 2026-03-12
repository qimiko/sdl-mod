cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

project(sdl-mod VERSION 1.0.2)

set(SOURCES
    src/main.mm
    src/input.cpp
    src/hooks.cpp
    src/keymap.cpp
    src/events.cpp
)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_source_files_properties(
    src/main.mm
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage(
    name SDL3
    GITHUB_REPOSITORY libsdl-org/SDL
    GIT_TAG release-3.4.2
    OPTIONS
        "SDL_AUDIO OFF"
        "SDL_GPU OFF"
        "SDL_RENDER OFF"
        "SDL_CAMERA OFF"
        "SDL_HAPTIC OFF"
        "SDL_POWER OFF"
        "SDL_SENSOR OFF"
        "SDL_DIALOG OFF"
        "SDL_TRAY OFF"
        "SDL_VULKAN OFF"
        "SDL_METAL OFF"
        "SDL_OPENGLES OFF"
)

target_link_libraries(${PROJECT_NAME} SDL3::SDL3-static)

setup_geode_mod(${PROJECT_NAME})
