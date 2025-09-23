include(FetchContent)

# Backup original flags
set(old_cxx_flags "${CMAKE_CXX_FLAGS}")
set(old_c_flags "${CMAKE_C_FLAGS}")
set(no_dev_warnings_backup "$CACHE{CMAKE_SUPPRESS_DEVELOPER_WARNINGS}")

# Suppress warnings globally
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
endif()

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE INTERNAL "" FORCE)

# glm
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG ${GLM_VERSION}
  SYSTEM
)
FetchContent_MakeAvailable(glm)

# spdlog
set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_BUILD_SHARED OFF)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG ${SPDLOG_VERSION}
  SYSTEM
)
FetchContent_MakeAvailable(spdlog)

# Cereal
set(JUST_INSTALL_CEREAL ON)
set(BUILD_DOC OFF)
set(BUILD_SANDBOX OFF)
FetchContent_Declare(
  Cereal
  GIT_REPOSITORY https://github.com/USCiLab/cereal.git
  GIT_TAG ${CEREAL_VERSION}
  SYSTEM
)
FetchContent_MakeAvailable(Cereal)

# readerwriterqueue
FetchContent_Declare(
  readerwriterqueue
  GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue
  GIT_TAG master
)
FetchContent_MakeAvailable(readerwriterqueue)

# Restore original flags
set(CMAKE_CXX_FLAGS "${old_cxx_flags}")
set(CMAKE_C_FLAGS "${old_c_flags}")
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ${no_dev_warnings_backup} CACHE INTERNAL "" FORCE)
