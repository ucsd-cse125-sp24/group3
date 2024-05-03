set(LIB_NAME game_gui_lib)

set(FILES
    imgs/img.cpp
    imgs/loader.cpp

    font/font.cpp
    font/loader.cpp

    widget/centertext.cpp
    widget/textinput.cpp
    widget/widget.cpp
    widget/dyntext.cpp
    widget/flexbox.cpp
    widget/staticimg.cpp

    gui.cpp

    ../../../dependencies/stb/stb_image.cpp
)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ../../../dependencies/stb) # include stb image loading header
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_include_directories(${LIB_NAME} PRIVATE ${BOOST_LIBRARY_INCLUDES})
target_link_libraries(${LIB_NAME} PRIVATE game_shared_lib)
target_include_directories(${LIB_NAME} PRIVATE ${OPENGL_INCLUDE_DIRS} glfw glm ${FREETYPE_INCLUDE_DIRS} "${CMAKE_BINARY_DIR}/_deps/glew-src/include")
target_link_libraries(${LIB_NAME} PRIVATE glm glfw libglew_static freetype)
target_link_libraries(${LIB_NAME} 
    PRIVATE
    Boost::asio
    Boost::filesystem
    Boost::thread
    Boost::program_options
    Boost::serialization
    nlohmann_json::nlohmann_json
)