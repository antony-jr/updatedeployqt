CMAKE_MINIMUM_REQUIRED( VERSION 3.2)
project(updatedeployqt)

# Let cmake know that this is a release build.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Include Directories.
include_directories(.)
include_directories(include)

add_executable(updatedeployqt src/qmake_process.c src/logger.c src/main.c include/logger.h include/qmake_process.h)
target_include_directories(updatedeployqt PUBLIC . include)
