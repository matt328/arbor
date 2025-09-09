project("vulkan-renderer")

set(vulkanrenderer_SRC
  src/Frame.cxx
  src/FrameManager.cxx
  src/PerFrameUploader.cxx
  src/RenderContext.cxx
)

add_library(${PROJECT_NAME} STATIC ${vulkanrenderer_SRC})
add_library(vulkan::renderer ALIAS ${PROJECT_NAME})

find_package(Vulkan REQUIRED)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  PRIVATE
  src
  ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  base-kit
  vulkan::common
  vulkan::core
  graphics::common
  engine::common
  ${Vulkan_LIBRARIES}
)
