project("engine-base")

set(enginebase_SRC
  src/EngineContext.cxx
)

add_library(${PROJECT_NAME} STATIC ${enginebase_SRC})
add_library(engine::base ALIAS engine-base)

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
  gameplay
  base-kit
  graphics::base
  engine::common
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  PRIVATE
  include/engine
)
