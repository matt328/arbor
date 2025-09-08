project("engine-common")

add_library(${PROJECT_NAME} INTERFACE)
add_library(engine::common ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  INTERFACE
  include
)

target_precompile_headers(${PROJECT_NAME}
  INTERFACE
  <glm/glm.hpp>
  <glm/ext/matrix_clip_space.hpp>
  <glm/gtc/quaternion.hpp>
)

target_link_libraries(${PROJECT_NAME}
  INTERFACE
  glm
  base-kit
)
