project("gameplay")

set(gameplay_SRC
  src/GameplayContext.cxx
  src/EntityManager.cxx
)

add_library(${PROJECT_NAME} STATIC ${gameplay_SRC})

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
  <algorithm>
  <any>
  <deque>
  <functional>
  <future>
  <map>
  <memory>
  <optional>
  <random>
  <string>
  <tuple>
  <typeindex>
  <variant>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  base-kit
  engine::base
  engine::common
  ${TRACY_LIB}
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  ${TRACY_INCLUDE_DIR}
)
