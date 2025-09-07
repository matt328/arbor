project("vulkan-core")

set(vulkancore_SRC
  src/CoreContext.cxx
  src/Device.cxx
  src/Instance.cxx
  src/PhysicalDevice.cxx
  src/Surface.cxx
  src/Swapchain.cxx
)

add_library(${PROJECT_NAME} OBJECT ${vulkancore_SRC})
add_library(vulkan::core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  PRIVATE
  src
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  base-kit
  graphics::common
)
