cmake_minimum_required(VERSION 3.14)
project("testarea" VERSION "0.0.1" LANGUAGES CXX)

set(testarea_SRC
  src/main.cxx
)

add_executable(${PROJECT_NAME} ${testarea_SRC})

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  base-kit
  engine:base
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
)

target_precompile_headers(${PROJECT_NAME}
  PRIVATE
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  .
)
