CMAKE_MINIMUM_REQUIRED( VERSION 3.2)
project(updatedeployqt)

# Let cmake know that this is a release build.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

# Include Directories.
include_directories(.)
include_directories(include)

add_executable(updatedeployqt src/qmake_process.c
			      src/logger.c
			      src/md5.c 
			      src/downloader.c
			      src/config_writer.c
			      src/main.c 
			      include/logger.h 
			      include/qmake_process.h
			      include/md5.h
			      include/downloader.h
			      include/config_writer.h)
target_link_libraries(updatedeployqt PUBLIC curl)
target_include_directories(updatedeployqt PUBLIC . include)
