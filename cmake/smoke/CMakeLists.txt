cmake_minimum_required(VERSION 3.14)
project(carmen_smoke C)

# Consumes an installed/extracted libcarmen SDK via find_package. Configure
# from CI with -DCMAKE_PREFIX_PATH=<sdk-root>; the SDK's
# lib/cmake/carmen/carmenConfig.cmake resolves the rest relative to itself.
find_package(carmen CONFIG REQUIRED)

add_executable(carmen_smoke main.c)
target_link_libraries(carmen_smoke PRIVATE carmen::carmen)
