project("graphics-base")

set(graphicsbase_SRC
  src/Placeholder.cxx
)

add_library(${PROJECT_NAME} STATIC ${graphicsbase_SRC})
add_library(graphics::base ALIAS graphics-base)

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
  vulkan
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
)
