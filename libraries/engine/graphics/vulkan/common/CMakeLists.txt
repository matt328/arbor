project("vulkan-common")

set(vulkancommon_SRC
  src/Placeholder.cxx
)

add_library(${PROJECT_NAME} STATIC ${vulkancommon_SRC})
add_library(vulkan::common ALIAS ${PROJECT_NAME})

find_package(Vulkan REQUIRED)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  ${Vulkan_INCLUDE_DIRS}
  PRIVATE
  src
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  glm
  base-kit
  engine::common
  PUBLIC
  cpptrace::cpptrace
)
