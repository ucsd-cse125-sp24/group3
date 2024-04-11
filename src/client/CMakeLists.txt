set(TARGET_NAME client)

set(FILES
    main.cpp
    client.cpp
    cube.cpp
    util.cpp
)

# Subprojects
# potential review later
add_subdirectory(../../dependencies/glad ${CMAKE_BINARY_DIR}/glad)
add_subdirectory(../../dependencies/glfw ${CMAKE_BINARY_DIR}/glfw)
add_subdirectory(../../dependencies/glm ${CMAKE_BINARY_DIR}/glm)
add_subdirectory(../../dependencies/imgui ${CMAKE_BINARY_DIR}/imgui)

add_executable(${TARGET_NAME} ${FILES} ${imgui-source})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_link_libraries(${TARGET_NAME} PRIVATE game_shared_lib)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

target_include_directories(${TARGET_NAME} PRIVATE ${OPENGL_INCLUDE_DIRS} glad glfw glm ${imgui-directory})
target_link_libraries(${TARGET_NAME} PRIVATE ${OPENGL_LIBRARIES} glad glfw glm)
