project("api")

add_library(${PROJECT_NAME} INTERFACE)

target_precompile_headers(${PROJECT_NAME}
  INTERFACE
  <algorithm>
  <any>
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
  INTERFACE
)

target_include_directories(${PROJECT_NAME}
  INTERFACE
  include

)
