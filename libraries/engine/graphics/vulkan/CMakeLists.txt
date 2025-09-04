project("vulkan")

set(vulkan_SRC
  src/VulkanContext.cxx
  src/api/Instance.cxx
  src/api/PhysicalDevice.cxx
  .
  .
  .
)

add_library(${PROJECT_NAME} STATIC ${vulkan_SRC})

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
)

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
  <filesystem>
  <fstream>
  <string_view>
  <string>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  base-kit
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  PRIVATE
  api/include
  renderer/include
  runtime/include
)
