project("graphics-vk")

set(graphicsvk_SRC
  src/GraphicsContext.cxx

  src/vk/Instance.cxx
  src/vk/PhysicalDevice.cxx
  src/vk/Device.cxx
  src/vk/Surface.cxx
)

add_library(${PROJECT_NAME} STATIC ${graphicsvk_SRC})

find_package(Vulkan REQUIRED)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  VK_USE_PLATFORM_WIN32_KHR
)

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
  <algorithm>
  <any>
  <deque>
  <functional>
  <future>
  <map>
  <memory>
  <optional>
  <random>
  <string>
  <tuple>
  <typeindex>
  <variant>

  <vulkan/vulkan.h>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  base-kit
  api
  engine
  ${Vulkan_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${Vulkan_INCLUDE_DIRS}
)
