project("vulkan-base")

set(vulkanbase_SRC
  src/VulkanContext.cxx
)

add_library(${PROJECT_NAME} STATIC ${vulkanbase_SRC})
add_library(vulkan::base ALIAS ${PROJECT_NAME})

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  base-kit
  graphics::base
  PRIVATE
  vulkan::core
  graphics::common
  engine::common
  vulkan::renderer
  vulkan::common
  vulkan::resources
  vulkan::assets
  ${TRACY_LIB}
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  PRIVATE
  src
  ${TRACY_INCLUDE_DIR}
)
