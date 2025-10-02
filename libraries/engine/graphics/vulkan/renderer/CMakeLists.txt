project("vulkan-renderer")

set(vulkanrenderer_SRC
  src/Frame.cxx
  src/FrameManager.cxx
  src/PerFrameUploader.cxx
  src/RenderContext.cxx
  src/FrameRenderer.cxx

  src/framegraph/FrameGraph.cxx
  src/framegraph/AliasRegistry.cxx

  src/framegraph/render-pass/pass/ForwardPass.cxx

  src/geometry/VirtualGeometryAllocator.cxx
  src/geometry/VirtualGeometryAllocator.cxx
  src/geometry/GeometryStream.cxx
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
  ${TRACY_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  base-kit
  vulkan::resources
  vulkan::common
  vulkan::core
  graphics::common
  engine::common
  readerwriterqueue
  ${Vulkan_LIBRARIES}
  ${TRACY_LIB}
)
