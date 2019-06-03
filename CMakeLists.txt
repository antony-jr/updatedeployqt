CMAKE_MINIMUM_REQUIRED( VERSION 2.8)
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


add_definitions(-DJSON_TRACK_SOURCE) # tell json-parser to track the line from json files.

if(GIT_COMMIT_STR)
	add_definitions(-DGIT_COMMIT_STR="${GIT_COMMIT_STR}")
endif()

add_executable(updatedeployqt src/qmake_process.c
			      src/logger.c
			      src/json.c
			      src/args_parser.c
			      src/config_manager.c
			      src/deploy_info.c
			      src/bridge_deployer.c
			      src/md5.c 
			      src/downloader.c
			      src/config_writer.c
			      src/main.c 
			      include/logger.h 
			      include/qmake_process.h
			      include/md5.h
			      include/deploy_info.h
			      include/bridge_deployer.h
			      include/args_parser.h
			      include/config_manager.h
			      include/json.h
			      include/downloader.h
			      include/config_writer.h)

target_link_libraries(updatedeployqt PUBLIC curl m)
target_include_directories(updatedeployqt PUBLIC . include)

install(
	TARGETS updatedeployqt
	RUNTIME DESTINATION bin
)
