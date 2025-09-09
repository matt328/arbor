project("vulkan-core")

set(vulkancore_SRC
  src/CoreContext.cxx
  src/Device.cxx
  src/Instance.cxx
  src/PhysicalDevice.cxx
  src/Surface.cxx
  src/Swapchain.cxx
)

add_library(${PROJECT_NAME} STATIC ${vulkancore_SRC})
add_library(vulkan::core ALIAS ${PROJECT_NAME})

find_package(Vulkan REQUIRED)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
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
