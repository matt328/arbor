project("graphics-common")

add_library(${PROJECT_NAME} INTERFACE)
add_library(graphics::common ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  INTERFACE
  include
)
