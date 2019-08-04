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

SET(source)
list(APPEND source            src/logger.c
	                      src/qt_version_info.c
			      src/json.c
			      src/utils.c
			      src/args_parser.c
			      src/config_manager.c
			      src/config_generator.c
			      src/deploy_info.c
			      src/bridge_deployer.c
			      src/library_deployer.c
			      src/injector.c
			      src/md5.c
			      src/main.c
			      src/json_builder.c
			      include/json_builder.h 
			      include/logger.h 
			      include/md5.h
			      include/utils.h
			      include/qt_version_info.h
			      include/deploy_info.h
			      include/injector.h
			      include/bridge_deployer.h
			      include/library_deployer.h
			      include/args_parser.h
			      include/config_manager.h
			      include/config_generator.h
			      include/json.h)



if(CACHE_DIR)
	message("-- Will be static linking with libgcc.")
	set(CMAKE_EXE_LINKER_FLAGS "--static -static-libgcc")

	include_directories(${CACHE_DIR})
	add_definitions(-DBINARIES_PACKAGED_HEADER_AVAL)

	list(APPEND source ${CACHE_DIR}/libqxcb_5_6_0.h
		           ${CACHE_DIR}/libqxcb_5_7_0.h
			   ${CACHE_DIR}/libqxcb_5_8_0.h
			   ${CACHE_DIR}/libqxcb_5_9_0.h
			   ${CACHE_DIR}/libqxcb_5_10_0.h
			   ${CACHE_DIR}/aiub.h)
endif()

if(GIT_COMMIT_STR)
	add_definitions(-DGIT_COMMIT_STR="${GIT_COMMIT_STR}")
endif()

add_executable(updatedeployqt ${source})
target_link_libraries(updatedeployqt PUBLIC m)
target_include_directories(updatedeployqt PUBLIC . include)

install(
	TARGETS updatedeployqt
	RUNTIME DESTINATION bin
)
