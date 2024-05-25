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

# Generate a "compile_commands.json" file for VScode to use 
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Tell CMake to ignore warnings from stuff in our dependencies. Surely GLM
# knows what it is doing... https://7tv.app/emotes/61e8fae862858c6406126ced
IF (NOT WIN32)
    set_property(
        DIRECTORY build 
        PROPERTY COMPILE_OPTIONS "-w"
    )
ENDIF()

# fix issues on gh action where object files are too large
if (MSVC)
  add_compile_options(/bigobj)
else ()
  add_compile_options(-Wa,-mbig-obj)
endif ()

# Add google test to CMake
add_subdirectory(dependencies/google-test) 


# If we need any compiler / linker flags, add them here
  SET(GCC_COMPILE_FLAGS "")
SET(GCC_LINK_FLAGS    "")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}")

project(game VERSION 1.0)

# Custom Variables
SET(INCLUDE_DIRECTORY ${PROJECT_SOURCE_DIR}/include)

# Dependencies

# For some reason I had to add this in two places to make it work on certain lab computers???

add_subdirectory(dependencies/boost) # include boost libraries that we use
add_subdirectory(dependencies/json) # include boost libraries that we use
add_subdirectory(dependencies/glm) # include glm library

add_subdirectory(src/shared) # define game_shared_lib
add_subdirectory(src/client) # create client executable
add_subdirectory(src/server) # create server executable
add_subdirectory(src/debugger) # create debugger executable

find_program(CPPCHECK "cppcheck")
add_custom_target(lint
    COMMAND ${CPPCHECK}
    # checks to enable
    --enable=warning,style,performance,portability
    # allow us to suppress individual lines with // cppcheck-suppress <check-name>
    --inline-suppr
    # return error exit code 1 on failures
    --error-exitcode=1
    # ignore certain warnings that are annoying
    --suppress=unusedVariable
    # tell it about our include directory
    -I ${CMAKE_SOURCE_DIR}/include
    # only check include and src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    # ignore test directories
    -i${CMAKE_SOURCE_DIR}/src/client/tests
    -i${CMAKE_SOURCE_DIR}/src/server/tests
    -i${CMAKE_SOURCE_DIR}/src/shared/tests
)

add_custom_target(pull_models
  COMMAND 
  gdown 1N7a5cDgMcXbPO0RtgznnEo-1XUfdMScM -O ${CMAKE_SOURCE_DIR}/assets/graphics --folder
)
