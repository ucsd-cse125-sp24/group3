include(FetchContent)

FetchContent_Declare(glew 
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake
    GIT_TAG glew-cmake-2.2.0
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(glew)

# find_package(GLEW 2.0 REQUIRED)
# target_link_libraries(Starting GLEW::GLEW)

