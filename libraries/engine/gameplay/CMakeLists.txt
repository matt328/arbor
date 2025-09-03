project("gameplay")

set(gameplay_SRC
  src/GameplayContext.cxx
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
  api
  engine
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
)
