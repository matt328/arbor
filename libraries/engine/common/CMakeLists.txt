project("engine-common")

add_library(${PROJECT_NAME} INTERFACE)
add_library(engine::common ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  INTERFACE
  include
)
