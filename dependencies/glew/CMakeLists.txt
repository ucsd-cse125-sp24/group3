include(FetchContent)

FetchContent_Declare(glew 
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake
    GIT_TAG glew-cmake-2.2.0
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(glew)

set(GLEW_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}/_deps/glew-src/include
    PARENT_SCOPE)
