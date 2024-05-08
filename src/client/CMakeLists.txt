set(TARGET_NAME client)
set(LIB_NAME game_client_lib)

# Subprojects
# potential review later
add_subdirectory(../../dependencies/glfw ${CMAKE_BINARY_DIR}/glfw)
add_subdirectory(../../dependencies/glm ${CMAKE_BINARY_DIR}/glm)
add_subdirectory(../../dependencies/glew ${CMAKE_BINARY_DIR}/glew)
add_subdirectory(../../dependencies/freetype ${CMAKE_BINARY_DIR}/freetype)
add_subdirectory(../../dependencies/sfml ${CMAKE_BINARY_DIR}/sfml)
add_subdirectory(../../dependencies/assimp ${CMAKE_BINARY_DIR}/assimp)
add_subdirectory(../../dependencies/stb ${CMAKE_BINARY_DIR}/stb)

add_subdirectory(gui)

set(FILES
    audiomanager.cpp
    camera.cpp
    client.cpp
    cube.cpp
    util.cpp
    lobbyfinder.cpp

    shader.cpp
    model.cpp
    lightsource.cpp
    renderable.cpp
)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_include_directories(${LIB_NAME}
    PRIVATE 
    ${BOOST_LIBRARY_INCLUDES}
    ${OPENGL_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    glfw
    glm
    ${GLEW_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${STB_INCLUDE_DIRS}
)
target_link_libraries(${LIB_NAME} 
    PRIVATE
    game_shared_lib
    game_gui_lib
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
    freetype
    sfml-audio
)

add_executable(${TARGET_NAME} main.cpp)
target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_include_directories(${TARGET_NAME}
    PRIVATE
    ${BOOST_LIBRARY_INCLUDES}
    ${OPENGL_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    glfw
    glm
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
    freetype
    sfml-audio
)

add_subdirectory(tests) # define client unit tests

if(WIN32)
    add_custom_command(
        TARGET ${TARGET_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${TARGET_NAME}>
        VERBATIM)
endif()

install(TARGETS ${TARGET_NAME})