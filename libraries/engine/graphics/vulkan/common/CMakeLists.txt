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
  PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  src
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
)
