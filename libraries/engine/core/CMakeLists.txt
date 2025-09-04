project("core")

set(core_SRC
  src/EngineContext.cxx
)

add_library(${PROJECT_NAME} STATIC ${core_SRC})

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
  engine
  gameplay
  graphics-vk
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include

)
