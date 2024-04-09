cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
       If you think this error is wrong, try clearing cache:
       `find -name CMakeCache.txt -delete`
")
endif()

# CMake Variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)

# If we need any compiler / linker flags, add them here
SET(GCC_COMPILE_FLAGS "")
SET(GCC_LINK_FLAGS    "")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}")

project(game VERSION 1.0)

# Custom Variables
SET(INCLUDE_DIRECTORY ${PROJECT_SOURCE_DIR}/include)

# Dependencies

add_subdirectory(deps/boost) # include boost libraries that we use

add_subdirectory(src/shared) # define game_shared_lib
add_subdirectory(src/client) # create client executable
add_subdirectory(src/server) # create server executable