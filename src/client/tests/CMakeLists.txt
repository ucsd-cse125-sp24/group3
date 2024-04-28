set(TARGET_NAME client_tests)

set(FILES
    hello_client_test.cpp
)

add_executable(${TARGET_NAME} ${FILES})
target_link_libraries(${TARGET_NAME} PRIVATE game_client_lib game_shared_lib)

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})

target_link_libraries(${TARGET_NAME} PUBLIC gtest_main)
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
target_include_directories(${TARGET_NAME} PRIVATE ${BOOST_LIBRARY_INCLUDES})
target_include_directories(${TARGET_NAME}
    PRIVATE
    ${OPENGL_INCLUDE_DIRS}
    glfw
    glm
    ${imgui-directory}
    ${GLEW_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${STB_INCLUDE_DIRS}
)

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
)

# setup make target
set(RUN_TESTS_TARGET "run_${TARGET_NAME}")
add_custom_target(${RUN_TESTS_TARGET})
add_custom_command(
    TARGET ${RUN_TESTS_TARGET} 
    COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}"
    ARGS "--output-on-failure"
)
add_dependencies(${RUN_TESTS_TARGET} ${TARGET_NAME})


