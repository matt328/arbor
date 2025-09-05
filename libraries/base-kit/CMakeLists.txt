project("base-kit")

set(basekit_SRC
  src/Preferences.cxx
)

add_library(${PROJECT_NAME} STATIC ${basekit_SRC})

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  SPDLOG_ENABLE_THREAD_NAME
)

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
  <cereal/archives/binary.hpp>
  <cereal/types/unordered_map.hpp>
  <cereal/types/string.hpp>
  <deque>
  <filesystem>
  <fstream>
  <string_view>
  <spdlog/spdlog.h>
  <spdlog/async.h>
  <spdlog/async_logger.h>
  <spdlog/sinks/basic_file_sink.h>
  <spdlog/sinks/stdout_color_sinks.h>
  <spdlog/details/registry.h>
  <string>
  <bk/Logger.hpp>
  <bk/NonCopyMove.hpp>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  spdlog
  cereal::cereal
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  PRIVATE
  src
)
