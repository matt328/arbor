project("engine")

add_subdirectory(api)
add_subdirectory(core)
add_subdirectory(gameplay)
add_subdirectory(graphics-vk)

set(engine_SRC
  public/Placeholder.cxx
)

add_library(${PROJECT_NAME} STATIC ${engine_SRC})

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
  api
  core
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  public
)
