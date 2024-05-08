include(FetchContent)

FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(stb)

set(STB_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}/_deps/stb-src/
    PARENT_SCOPE)
