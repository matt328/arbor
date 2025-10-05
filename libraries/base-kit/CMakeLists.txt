project("base-kit")

set(basekit_SRC
  src/Preferences.cxx
  src/Log.cxx
)

add_library(${PROJECT_NAME} STATIC ${basekit_SRC})

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  cereal::cereal
  PUBLIC
  quill::quill
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  PRIVATE
  src
)
