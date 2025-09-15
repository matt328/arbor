project("vulkan-resources")

set(vulkanresources_SRC
  src/ResourceContext.cxx
  src/ResourceFacade.cxx

  src/buffers/BufferManager.cxx
  src/buffers/ArenaAllocator.cxx
  src/buffers/BufferAllocator.cxx
  src/buffers/LinearAllocator.cxx
  src/buffers/BufferPool.cxx
)

add_library(${PROJECT_NAME} STATIC ${vulkanresources_SRC})
add_library(vulkan::resources ALIAS ${PROJECT_NAME})

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
  vulkan::core
  PUBLIC
  base-kit
  vulkan::common
  engine::common
  ${Vulkan_LIBRARIES}
)
