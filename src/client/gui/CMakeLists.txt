set(LIB_NAME game_gui_lib)

set(FILES
    imgs/img.cpp
    imgs/loader.cpp
    imgs/logo.cpp

    font/font.cpp
    font/loader.cpp

    widget/centertext.cpp
    widget/textinput.cpp
    widget/widget.cpp
    widget/dyntext.cpp
    widget/flexbox.cpp
    widget/staticimg.cpp
    widget/empty.cpp

    gui.cpp
)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_link_libraries(${LIB_NAME} PRIVATE game_shared_lib)
target_include_directories(${LIB_NAME} PRIVATE 
    ${OPENGL_INCLUDE_DIRS} 
    ${BOOST_LIBRARY_INCLUDES}
    ${STB_INCLUDE_DIRS}
    glfw 
    glm 
    ${FREETYPE_INCLUDE_DIRS} 
    ${ASSIMP_INCLUDE_DIRS} 
    sfml-audio
    "${CMAKE_BINARY_DIR}/_deps/glew-src/include"
)
target_link_libraries(${LIB_NAME} PRIVATE glm glfw libglew_static freetype)
target_link_libraries(${LIB_NAME} 
    PRIVATE
    Boost::asio
    Boost::filesystem
    Boost::thread
    Boost::program_options
    Boost::serialization
    nlohmann_json::nlohmann_json
    assimp
    sfml-audio
)