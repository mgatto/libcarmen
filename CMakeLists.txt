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
#  Sanitizers (ASan + UBSan, GCC/Clang only)
#
#  -Wall -Wextra -pedantic catch style/type mistakes, not undefined behavior.
#  Configure with -DCARMEN_SANITIZE=address,undefined to build the library,
#  Unity, and every test binary with sanitizer instrumentation -- see
#  .cursor/rules/c-safety.mdc. Applied globally via add_compile_options
#  rather than per-target so it doesn't need to be threaded through every
#  add_library/add_executable call individually.
# --------------------------------------------------------------------------- #

set(CARMEN_SANITIZE "" CACHE STRING
    "Comma-separated sanitizers to build with, e.g. address,undefined (GCC/Clang only, ignored when cross-compiling)")

if(CARMEN_SANITIZE)
    if(MSVC)
        message(WARNING "CARMEN_SANITIZE is ignored on MSVC; use MSVC's own /fsanitize=address separately")
    elseif(CMAKE_CROSSCOMPILING)
        message(WARNING "CARMEN_SANITIZE is ignored when cross-compiling (e.g. emscripten)")
    else()
        add_compile_options(-fsanitize=${CARMEN_SANITIZE} -fno-omit-frame-pointer -g -O0)
        add_link_options(-fsanitize=${CARMEN_SANITIZE})
    endif()
endif()

# --------------------------------------------------------------------------- #
#  Built-in world code generation
#
#  presets/islamic.jsonc is the single source of truth for the built-in world.
#  A host tool (tools/gen_world) validates it and emits a C source defining
#  carmen_world_build_islamic().  Editing the preset regenerates that source,
#  so schema/capacity/reference errors fail the build (there is no runtime
#  world loader).  WASM is a first-class target: when cross-compiling, the
#  generator is built with the host toolchain via ExternalProject (it must run
#  on the build machine); otherwise it is an ordinary target.
# --------------------------------------------------------------------------- #

set(CARMEN_PRESET        ${CMAKE_CURRENT_SOURCE_DIR}/presets/islamic.jsonc)
set(CARMEN_GEN_DIR       ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(CARMEN_GEN_WORLD_SRC ${CARMEN_GEN_DIR}/world_islamic_generated.c)

if(CMAKE_CROSSCOMPILING)
    include(ExternalProject)
    ExternalProject_Add(carmen_host_tools
        SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/tools
        CMAKE_ARGS      -DCMAKE_BUILD_TYPE=Release
        # Clear any inherited cross toolchain (e.g. emscripten) so the
        # generator is built as a native host executable that can run here.
        CMAKE_CACHE_DEFAULT_ARGS -DCMAKE_TOOLCHAIN_FILE:FILEPATH=
        INSTALL_COMMAND ""
        BUILD_ALWAYS    ON
    )
    ExternalProject_Get_Property(carmen_host_tools BINARY_DIR)
    set(CARMEN_GEN_WORLD_EXE ${BINARY_DIR}/gen_world)
    set(CARMEN_GEN_WORLD_DEP carmen_host_tools)
else()
    add_subdirectory(tools)
    set(CARMEN_GEN_WORLD_EXE $<TARGET_FILE:gen_world>)
    set(CARMEN_GEN_WORLD_DEP gen_world)
endif()

add_custom_command(
    OUTPUT  ${CARMEN_GEN_WORLD_SRC}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CARMEN_GEN_DIR}
    COMMAND ${CARMEN_GEN_WORLD_EXE} ${CARMEN_PRESET} ${CARMEN_GEN_WORLD_SRC}
    DEPENDS ${CARMEN_GEN_WORLD_DEP} ${CARMEN_PRESET}
    COMMENT "Generating built-in world from presets/islamic.jsonc"
    VERBATIM
)

# --------------------------------------------------------------------------- #
#  Library
# --------------------------------------------------------------------------- #

set(CARMEN_FRIBIDI_SOURCES
    vendor/fribidi/lib/fribidi.c
    vendor/fribidi/lib/fribidi-arabic.c
    vendor/fribidi/lib/fribidi-bidi.c
    vendor/fribidi/lib/fribidi-bidi-types.c
    vendor/fribidi/lib/fribidi-char-sets.c
    vendor/fribidi/lib/fribidi-char-sets-cap-rtl.c
    vendor/fribidi/lib/fribidi-char-sets-cp1255.c
    vendor/fribidi/lib/fribidi-char-sets-cp1256.c
    vendor/fribidi/lib/fribidi-char-sets-iso8859-6.c
    vendor/fribidi/lib/fribidi-char-sets-iso8859-8.c
    vendor/fribidi/lib/fribidi-char-sets-utf8.c
    vendor/fribidi/lib/fribidi-deprecated.c
    vendor/fribidi/lib/fribidi-joining.c
    vendor/fribidi/lib/fribidi-joining-types.c
    vendor/fribidi/lib/fribidi-mirroring.c
    vendor/fribidi/lib/fribidi-brackets.c
    vendor/fribidi/lib/fribidi-run.c
    vendor/fribidi/lib/fribidi-shape.c
)

set(CARMEN_SOURCES
    src/utf8.c
    src/site.c
    src/connection.c
    src/city.c
    src/game_world.c
    src/connection_gen.c
    ${CARMEN_GEN_WORLD_SRC}
    src/villain.c
    src/artifact.c
    src/case.c
    src/session.c
    src/settings.c
    src/save.c
    src/i18n.c
    vendor/cjson/cJSON.c
    ${CARMEN_FRIBIDI_SOURCES}
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
    include/carmen/world_islamic.h
    include/carmen/villain.h
    include/carmen/artifact.h
    include/carmen/case.h
    include/carmen/session.h
    include/carmen/settings.h
    include/carmen/save.h
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
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/utf8
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cjson
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/toml-c
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/fribidi
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/fribidi/lib
)

target_compile_definitions(carmen PRIVATE HAVE_CONFIG_H)

if(NOT MSVC)
    target_link_libraries(carmen PUBLIC m)
endif()

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

    # Make the demo look for the shared lib next to itself at runtime, so the
    # dist bundle is relocatable. Meaningless on Windows (DLLs resolve from the
    # executable's directory) and for static builds, but harmless there.
    if(APPLE)
        set(_carmen_rpath "@loader_path")
    else()
        set(_carmen_rpath "$ORIGIN")
    endif()
    if(NOT WIN32)
        # BUILD_WITH_INSTALL_RPATH bakes only INSTALL_RPATH into the built
        # binary and skips CMake's automatic build-tree rpath, so the bundle
        # stays relocatable (no absolute build path leaks in).
        set_target_properties(trail_demo PROPERTIES
            INSTALL_RPATH            "${_carmen_rpath}"
            BUILD_WITH_INSTALL_RPATH TRUE
        )
    endif()

    # ----------------------------------------------------------------------- #
    #  Distributable bundle
    #
    #  Assemble a self-contained, runnable folder in the source tree that
    #  survives cleaning the build directory: the demo, the carmen library it
    #  links, the locales it reads from the CWD, and an editable settings file
    #  (never clobbered on re-runs). Build it with:
    #      cmake --build <build-dir> --target dist
    #  For a bundle that actually loads the shipped shared lib at runtime,
    #  configure with -DBUILD_SHARED_LIBS=ON.
    # ----------------------------------------------------------------------- #

    set(CARMEN_DIST_DIR "${CMAKE_SOURCE_DIR}/dist")

    set(_dist_commands
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CARMEN_DIST_DIR}/locales"
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE:trail_demo> "${CARMEN_DIST_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE:carmen> "${CARMEN_DIST_DIR}/"
    )
    # On Unix shared builds the demo's load command names the soname
    # (e.g. libcarmen.0.dylib / libcarmen.so.0), so ship that name too.
    if(BUILD_SHARED_LIBS AND NOT WIN32)
        list(APPEND _dist_commands
            COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_SONAME_FILE:carmen> "${CARMEN_DIST_DIR}/"
        )
    endif()
    list(APPEND _dist_commands
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/locales" "${CARMEN_DIST_DIR}/locales"
        COMMAND ${CMAKE_COMMAND}
                -DSRC=${CMAKE_SOURCE_DIR}/examples/settings.default.toml
                -DDST=${CARMEN_DIST_DIR}/settings.toml
                -P ${CMAKE_SOURCE_DIR}/cmake/copy_if_absent.cmake
        COMMAND ${CMAKE_COMMAND} -E echo
                "Bundle ready: cd ${CARMEN_DIST_DIR} && ./trail_demo en settings.toml"
    )

    add_custom_target(dist
        ${_dist_commands}
        DEPENDS trail_demo carmen
        COMMENT "Assembling runnable dist bundle in ${CARMEN_DIST_DIR}"
        VERBATIM
    )

    # ----------------------------------------------------------------------- #
    #  Windows demo zip
    #
    #  Counterpart to `make package` on macOS. Wipes dist/ first so a stale
    #  or edited bundle cannot leak in, restages a self-named folder, and
    #  zips it with cmake -E tar (no extra tooling). Windows-only so the
    #  artifact name never collides with the macOS tarball.
    # ----------------------------------------------------------------------- #
    if(WIN32)
        string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _carmen_proc)
        if(_carmen_proc MATCHES "^(amd64|x86_64|x64)$")
            set(CARMEN_DEMO_ARCH x64)
        elseif(_carmen_proc MATCHES "^(arm64|aarch64)$")
            set(CARMEN_DEMO_ARCH arm64)
        else()
            set(CARMEN_DEMO_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
        endif()

        set(CARMEN_DEMO_PKG_NAME
            "libcarmen-demo-${PROJECT_VERSION}-windows-${CARMEN_DEMO_ARCH}")
        set(CARMEN_DEMO_PKG_DIR
            "${CMAKE_BINARY_DIR}/${CARMEN_DEMO_PKG_NAME}")
        set(CARMEN_DEMO_ZIP
            "${CMAKE_BINARY_DIR}/${CARMEN_DEMO_PKG_NAME}.zip")

        if(NOT BUILD_SHARED_LIBS)
            message(WARNING
                "demo_package is intended for -DBUILD_SHARED_LIBS=ON so the zip contains carmen.dll next to trail_demo.exe")
        endif()

        set(CARMEN_DEMO_README "${CMAKE_BINARY_DIR}/demo_README.txt")
        file(WRITE "${CARMEN_DEMO_README}"
"libcarmen demo (${PROJECT_VERSION}, windows ${CARMEN_DEMO_ARCH})

Run from inside this folder:
  trail_demo.exe en settings.toml

carmen.dll must stay next to trail_demo.exe. The demo loads
locales/en.json relative to the current directory.

If Windows reports a missing VCRUNTIME or MSVCP DLL, install
the Microsoft Visual C++ Redistributable for 64-bit Windows.
")

        add_custom_target(demo_package
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${CARMEN_DIST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CARMEN_DIST_DIR}/locales"
            COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE:trail_demo> "${CARMEN_DIST_DIR}/"
            COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE:carmen> "${CARMEN_DIST_DIR}/"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_SOURCE_DIR}/locales" "${CARMEN_DIST_DIR}/locales"
            COMMAND ${CMAKE_COMMAND} -E copy
                    "${CMAKE_SOURCE_DIR}/examples/settings.default.toml"
                    "${CARMEN_DIST_DIR}/settings.toml"
            COMMAND ${CMAKE_COMMAND} -E copy
                    "${CMAKE_SOURCE_DIR}/LICENSE" "${CARMEN_DIST_DIR}/"
            COMMAND ${CMAKE_COMMAND} -E copy
                    "${CARMEN_DEMO_README}" "${CARMEN_DIST_DIR}/README.txt"
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${CARMEN_DEMO_PKG_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CARMEN_DIST_DIR}" "${CARMEN_DEMO_PKG_DIR}"
            COMMAND ${CMAKE_COMMAND} -E tar cf "${CARMEN_DEMO_ZIP}"
                    --format=zip -- "${CARMEN_DEMO_PKG_NAME}"
            COMMAND ${CMAKE_COMMAND} -E echo "Package ready: ${CARMEN_DEMO_ZIP}"
            DEPENDS dist trail_demo carmen
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Packaging Windows demo zip ${CARMEN_DEMO_PKG_NAME}.zip"
            VERBATIM
        )
    endif()
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
    carmen_add_test(test_utf8              test/test_utf8.c)
    carmen_add_test(test_game_world        test/test_game_world.c)
    carmen_add_test(test_connection_gen    test/test_connection_gen.c)
    carmen_add_test(test_carmen_scenarios  test/test_carmen_scenarios.c)
    carmen_add_test(test_artifact          test/test_artifact.c)
    carmen_add_test(test_case              test/test_case.c)
    carmen_add_test(test_session            test/test_session.c)
    carmen_add_test(test_save              test/test_save.c)
    carmen_add_test(test_settings          test/test_settings.c)
    carmen_add_test(test_world_islamic     test/test_world_islamic.c)
    carmen_add_test(test_i18n              test/test_i18n.c)

    # The generator must reject invalid presets at build time.  These cases
    # only apply when gen_world is an ordinary (native) target.
    if(NOT CMAKE_CROSSCOMPILING)
        foreach(_bad bad_json over_cap_sites leftover_routes)
            add_test(NAME gen_world_rejects_${_bad}
                COMMAND gen_world
                        ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures/${_bad}.jsonc
                        ${CMAKE_CURRENT_BINARY_DIR}/gen_reject_${_bad}.c)
            set_tests_properties(gen_world_rejects_${_bad}
                PROPERTIES WILL_FAIL TRUE)
        endforeach()
    endif()
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
