project("engine-base")

set(enginebase_SRC
  src/EngineContext.cxx
)

add_library(${PROJECT_NAME} STATIC ${enginebase_SRC})
add_library(engine::base ALIAS engine-base)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  gameplay
  base-kit
  graphics::base
  graphics::common
  engine::common
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
)
