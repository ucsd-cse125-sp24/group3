include(FetchContent)

FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp
    GIT_TAG v5.3.1
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(assimp)

set(ASSIMP_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}/_deps/assimp-src/include/
    ${CMAKE_BINARY_DIR}/_deps/assimp-build/include/
    PARENT_SCOPE
)
