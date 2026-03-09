cmake_minimum_required(VERSION 3.14)
project(carmen
    VERSION     0.1.0
    LANGUAGES   C
    DESCRIPTION "C library for building Carmen Sandiego-style world exploration games"
)

# --------------------------------------------------------------------------- #
#  Options
# --------------------------------------------------------------------------- #

option(CARMEN_BUILD_TESTS    "Build unit tests (Unity)"     ON)
option(CARMEN_BUILD_EXAMPLES "Build example programs"       ON)

# --------------------------------------------------------------------------- #
#  C standard
# --------------------------------------------------------------------------- #

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# --------------------------------------------------------------------------- #
#  Library
# --------------------------------------------------------------------------- #

set(CARMEN_SOURCES
    src/utf8.c
    src/site.c
    src/connection.c
    src/city.c
    src/game_world.c
    src/seed_data_islamic.c
    src/villain.c
    src/i18n.c
    vendor/cjson/cJSON.c
)

set(CARMEN_PUBLIC_HEADERS
    include/carmen/carmen.h
    include/carmen/carmen_export.h
    include/carmen/carmen_version.h
    include/carmen/utf8.h
    include/carmen/clue.h
    include/carmen/site.h
    include/carmen/connection.h
    include/carmen/city.h
    include/carmen/game_world.h
    include/carmen/seed_data_islamic.h
    include/carmen/villain.h
    include/carmen/i18n.h
)

# BUILD_SHARED_LIBS (standard CMake variable) controls static vs shared.
# Default to static if not set.
add_library(carmen ${CARMEN_SOURCES})
add_library(carmen::carmen ALIAS carmen)

target_include_directories(carmen
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/utf8
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cjson
)

set_target_properties(carmen PROPERTIES
    VERSION                 ${PROJECT_VERSION}
    SOVERSION               ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER           "${CARMEN_PUBLIC_HEADERS}"
    C_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Shared-library plumbing: define CARMEN_DLL_EXPORT when building the DLL,
# and tell consumers to define CARMEN_DLL so they get __declspec(dllimport).
# On GCC/Clang these defines are harmless (export.h checks _WIN32 first).
if(BUILD_SHARED_LIBS)
    target_compile_definitions(carmen
        PRIVATE   CARMEN_DLL_EXPORT
        INTERFACE CARMEN_DLL
    )
endif()

# MSVC: suppress CRT deprecation warnings for strncpy et al.
if(MSVC)
    target_compile_definitions(carmen PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(carmen PRIVATE /W4)
else()
    target_compile_options(carmen PRIVATE -Wall -Wextra -pedantic)
endif()

# --------------------------------------------------------------------------- #
#  Example binary
# --------------------------------------------------------------------------- #

if(CARMEN_BUILD_EXAMPLES)
    add_executable(trail_demo examples/trail_demo.c)
    target_link_libraries(trail_demo PRIVATE carmen)
endif()

# --------------------------------------------------------------------------- #
#  Tests (Unity)
# --------------------------------------------------------------------------- #

if(CARMEN_BUILD_TESTS)
    enable_testing()

    add_library(unity STATIC vendor/unity/unity.c)
    target_include_directories(unity PUBLIC vendor/unity)
    target_compile_definitions(unity PUBLIC UNITY_INCLUDE_DOUBLE)
    if(MSVC)
        target_compile_definitions(unity PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()

    function(carmen_add_test test_name test_source)
        add_executable(${test_name} ${test_source})
        target_link_libraries(${test_name} PRIVATE carmen unity)
        add_test(NAME ${test_name} COMMAND ${test_name})
    endfunction()

    carmen_add_test(test_site              test/test_site.c)
    carmen_add_test(test_connection        test/test_connection.c)
    carmen_add_test(test_city              test/test_city.c)
    carmen_add_test(test_game_world        test/test_game_world.c)
    carmen_add_test(test_carmen_scenarios  test/test_carmen_scenarios.c)
endif()

# --------------------------------------------------------------------------- #
#  Install
# --------------------------------------------------------------------------- #

include(GNUInstallDirs)

install(TARGETS carmen
    EXPORT          carmenTargets
    ARCHIVE         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME         DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/carmen
)

# CMake package config so find_package(carmen) works
install(EXPORT carmenTargets
    FILE        carmenTargets.cmake
    NAMESPACE   carmen::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/carmen
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/carmenConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/carmenConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/carmen
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/carmenConfigVersion.cmake"
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/carmenConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/carmenConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/carmen
)

# pkg-config
set(PREFIX "${CMAKE_INSTALL_PREFIX}")
set(VERSION "${PROJECT_VERSION}")
configure_file(carmen.pc.in "${CMAKE_CURRENT_BINARY_DIR}/carmen.pc" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/carmen.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
