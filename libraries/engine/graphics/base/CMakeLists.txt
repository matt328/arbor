project("graphics-base")

set(graphicsbase_SRC
  src/Placeholder.cxx
)

add_library(${PROJECT_NAME} STATIC ${graphicsbase_SRC})
add_library(graphics::base ALIAS graphics-base)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  base-kit
  graphics::common
  engine::common
  vulkan::base
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
)
