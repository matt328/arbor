project("vulkan-renderer")

set(vulkanrenderer_SRC
  src/Frame.cxx
  src/FrameManager.cxx
  src/PerFrameUploader.cxx
  src/RenderContext.cxx
  src/FrameRenderer.cxx
  src/GlobalBufferManager.cxx

  src/buffers/ArenaAllocator.cxx
  src/buffers/BufferSystem.cxx
  src/buffers/LinearAllocator.cxx
  src/buffers/LogicalBufferMapper.cxx

  src/framegraph/FrameGraph.cxx
  src/framegraph/AliasRegistry.cxx

  src/framegraph/render-pass/pass/CullingPass.cxx
  src/framegraph/render-pass/pass/ForwardPass.cxx
  src/framegraph/render-pass/pass/PresentPass.cxx

  src/geometry/VirtualGeometryAllocator.cxx
  src/geometry/VirtualGeometryAllocator.cxx
  src/geometry/GeometryStream.cxx

  src/images/ImageManager.cxx
  src/images/ImageSystem.cxx
  src/images/ImageViewManager.cxx
  src/images/LogicalImageAllocator.cxx
  src/images/LogicalImageViewAllocator.cxx
  src/images/Sampler.cxx
  src/images/SamplerManager.cxx

  src/transfer/TransferSystem.cxx
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
  vulkan::common
  vulkan::core
  graphics::common
  engine::common
  readerwriterqueue
  ${Vulkan_LIBRARIES}
  ${TRACY_LIB}
)
