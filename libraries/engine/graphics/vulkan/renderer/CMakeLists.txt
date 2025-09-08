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

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
  <filesystem>
  <fstream>
  <limits>
  <string_view>
  <string>
  <variant>

  <vulkan/vulkan.h>
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  ${Vulkan_INCLUDE_DIRS}
  PRIVATE
  src
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  base-kit
  vulkan::common
  vulkan::core
  graphics::common
  engine::common
  ${Vulkan_LIBRARIES}
)
