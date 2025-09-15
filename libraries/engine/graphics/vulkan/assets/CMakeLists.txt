project("vulkan-assets")

set(vulkanassets_SRC
  src/AssetContext.cxx
)

add_library(${PROJECT_NAME} STATIC ${vulkanassets_SRC})
add_library(vulkan::assets ALIAS ${PROJECT_NAME})

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
  PUBLIC
  base-kit
  ${Vulkan_LIBRARIES}
)
