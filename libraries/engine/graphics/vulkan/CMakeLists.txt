project("vulkan")

add_subdirectory(core)

set(vulkan_SRC
  base/src/VulkanContext.cxx

  renderer/src/FrameManager.cxx
  renderer/src/RenderContext.cxx
  renderer/src/Frame.cxx
  renderer/src/PerFrameUploader.cxx
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
  <variant>

  <glm/glm.hpp>
  <glm/ext/matrix_clip_space.hpp>
  <glm/gtc/quaternion.hpp>

  <vulkan/vulkan.h>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  glm
  graphics::base
  graphics::common
  engine::common
  ${Vulkan_LIBRARIES}
  PRIVATE
  vulkan::core
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  base/include
  ${Vulkan_INCLUDE_DIRS}
  PRIVATE
  base/src
  renderer/include
  runtime/include
  common/include
  common/src
)
