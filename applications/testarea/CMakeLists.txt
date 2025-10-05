cmake_minimum_required(VERSION 3.14)
project("testarea" VERSION "0.0.1" LANGUAGES CXX)

set(testarea_SRC
  src/main.cxx
  src/Log.cxx
)

add_executable(${PROJECT_NAME} ${testarea_SRC})

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  base-kit
  engine::base
  engine::common
  quill::quill
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
  .
)
