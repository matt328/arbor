project("vulkan")

set(vulkan_SRC
  base/src/VulkanContext.cxx

  core/src/CoreContext.cxx
  core/src/Device.cxx
  core/src/Instance.cxx
  core/src/PhysicalDevice.cxx
  core/src/Surface.cxx
  core/src/Swapchain.cxx
)

add_library(${PROJECT_NAME} STATIC ${vulkan_SRC})

find_package(Vulkan REQUIRED)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  VK_USE_PLATFORM_WIN32_KHR
)

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
  <filesystem>
  <fstream>
  <limits>
  <string_view>
  <string>

  <vulkan/vulkan.h>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  base-kit
  graphics::base
  graphics::common
  ${Vulkan_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  base/include
  ${Vulkan_INCLUDE_DIRS}
  PRIVATE
  base/src
  renderer/include
  runtime/include
  core/include
  core/src
)
