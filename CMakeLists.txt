cmake_minimum_required(VERSION 3.10)

project(arbor LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;")

include(FetchContent)

set(ENABLE_ALL_WARNINGS ON)
set(ENABLE_EFFECTIVE_CXX ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

set(SPDLOG_VERSION v1.15.3)
set(CEREAL_VERSION v1.3.2)

include(dependencies)

add_subdirectory(libraries/base-kit)
add_subdirectory(libraries/engine)
add_subdirectory(applications/testarea)
