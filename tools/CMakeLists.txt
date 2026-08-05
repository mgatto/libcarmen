# Host build tool: compiles a world preset (presets/*.jsonc) into a C source.
#
# This file is deliberately self-contained so it can be configured on its own
# with the *host* toolchain via ExternalProject when the main project is being
# cross-compiled (e.g. emscripten), where the generator itself must still run
# on the build machine.
cmake_minimum_required(VERSION 3.14)
project(carmen_gen_world LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(_root ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(gen_world
    gen_world.c
    ${_root}/vendor/cjson/cJSON.c
)

target_include_directories(gen_world PRIVATE
    ${_root}/vendor/cjson
    ${_root}/include
)

if(MSVC)
    target_compile_definitions(gen_world PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(gen_world PRIVATE /W4)
else()
    target_compile_options(gen_world PRIVATE -Wall -Wextra -pedantic)
endif()
