project("engine-common")

set(enginecommon_SRC
  src/Placeholder.cxx
)

add_library(${PROJECT_NAME} STATIC ${enginecommon_SRC})
add_library(engine::common ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  ${TRACY_INCLUDE_DIR}
  PRIVATE
  src
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  base-kit
  ${TRACY_LIB}
  PUBLIC
  glm
)
