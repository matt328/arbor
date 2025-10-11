project("vulkan-core")

set(vulkancore_SRC
  src/CoreContext.cxx
  src/Device.cxx
  src/Instance.cxx
  src/PhysicalDevice.cxx
  src/Surface.cxx
  src/Swapchain.cxx
  src/AllocatorService.cxx
  src/Image.cxx
  src/ImageView.cxx
  src/CommandBuffer.cxx
  src/Buffer.cxx
  src/command-buffer/CommandBufferManager.cxx
  src/shader-binding/DSLayout.cxx

  src/pipeline/PipelineManager.cxx
  src/pipeline/SpirvShaderModuleFactory.cxx
  src/pipeline/Pipeline.cxx
  src/pipeline/PipelineLayout.cxx
  src/pipeline/ShaderModule.cxx
)

add_library(${PROJECT_NAME} STATIC ${vulkancore_SRC})
add_library(vulkan::core ALIAS ${PROJECT_NAME})

find_package(Vulkan REQUIRED)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  WIN32_LEAN_AND_MEAN
  VK_USE_PLATFORM_WIN32_KHR
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  src
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  base-kit
  vulkan::common
  graphics::common
  engine::common
  ${Vulkan_LIBRARIES}
)
