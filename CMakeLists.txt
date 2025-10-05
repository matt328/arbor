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

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vendor/bin/QWindowKit")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS DEBUG TRACY_ENABLE)

find_package(cpptrace REQUIRED PATHS ${CMAKE_SOURCE_DIR}/vendor/src/cpptrace)
find_package(quill REQUIRED PATHS ${CMAKE_SOURCE_DIR}/vendor/src/quill)

if(WIN32)
  add_compile_definitions(NOMINMAX)
endif()

set(SPDLOG_VERSION v1.15.3)
set(CEREAL_VERSION v1.3.2)
set(GLM_VERSION 1.0.1)

set(TRACY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/vendor/src/tracy/include CACHE PATH "Tracy include dir")
set(TRACY_LIB ${CMAKE_SOURCE_DIR}/vendor/src/tracy/lib/win64/TracyClient.lib CACHE FILEPATH "Tracy static lib")

include(dependencies)

add_subdirectory(libraries/base-kit)
add_subdirectory(libraries/engine)
add_subdirectory(applications/testarea)
add_subdirectory(assets)
add_subdirectory(applications/glade)
