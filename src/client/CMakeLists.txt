set(TARGET_NAME client)
set(LIB_NAME game_client_lib)

set(FILES
    camera.cpp
    client.cpp
    cube.cpp
    util.cpp
    lobbyfinder.cpp
    shader.cpp
    model.cpp
    ${imgui-source}
)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Subprojects
# potential review later
add_subdirectory(../../dependencies/glfw ${CMAKE_BINARY_DIR}/glfw)
add_subdirectory(../../dependencies/glm ${CMAKE_BINARY_DIR}/glm)
add_subdirectory(../../dependencies/imgui ${CMAKE_BINARY_DIR}/imgui)
add_subdirectory(../../dependencies/glew ${CMAKE_BINARY_DIR}/glew)
add_subdirectory(../../dependencies/assimp ${CMAKE_BINARY_DIR}/assimp)
add_subdirectory(../../dependencies/stb ${CMAKE_BINARY_DIR}/stb)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_include_directories(${LIB_NAME}
    PRIVATE 
    ${BOOST_LIBRARY_INCLUDES}
    ${OPENGL_INCLUDE_DIRS}
    glfw
    glm
    ${imgui-directory}
    ${GLEW_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${STB_INCLUDE_DIRS}
)
target_link_libraries(${LIB_NAME} 
    PRIVATE
    game_shared_lib
    Boost::asio
    Boost::filesystem
    Boost::thread
    Boost::program_options
    Boost::serialization
    nlohmann_json::nlohmann_json
    glm
    glfw
    libglew_static
    assimp
)

add_executable(${TARGET_NAME} main.cpp)
target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_include_directories(${TARGET_NAME}
    PRIVATE
    ${BOOST_LIBRARY_INCLUDES}
    ${OPENGL_INCLUDE_DIRS}
    glfw
    glm
    ${imgui-directory}
    ${GLEW_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${STB_INCLUDE_DIRS}
)
target_link_libraries(${TARGET_NAME} PRIVATE game_shared_lib ${LIB_NAME})
target_link_libraries(${TARGET_NAME}
    PRIVATE
    Boost::asio
    Boost::filesystem
    Boost::thread
    Boost::program_options
    Boost::serialization
    nlohmann_json::nlohmann_json
    glm
    glfw
    libglew_static
    assimp
)

add_subdirectory(tests) # define client unit tests
