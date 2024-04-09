set(TARGET_NAME client)

set(FILES
    main.cpp
    client.cpp
)

add_executable(${TARGET_NAME} ${FILES})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_link_libraries(${TARGET_NAME} PRIVATE game_shared_lib)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)


# Subprojects
# potential review later
add_subdirectory(../../dependencies/glad ${CMAKE_BINARY_DIR}/glad)
add_subdirectory(../../dependencies/glfw ${CMAKE_BINARY_DIR}/glfw)

target_include_directories(${TARGET_NAME} PRIVATE ${OPENGL_INCLUDE_DIRS} glad glfw)

target_link_libraries(${TARGET_NAME} PRIVATE ${OPENGL_LIBRARIES} glad glfw)
