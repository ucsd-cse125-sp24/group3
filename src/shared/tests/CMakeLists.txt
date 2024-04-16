set(TARGET_NAME shared_tests)

set(FILES
    hello_shared_test.cpp
)

add_executable(${TARGET_NAME} ${FILES})
target_link_libraries(${TARGET_NAME} PRIVATE game_shared_lib)

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_include_directories(${TARGET_NAME} PRIVATE ${BOOST_LIBRARY_INCLUDES})
target_link_libraries(${TARGET_NAME} 
    PRIVATE 
    Boost::asio
    Boost::filesystem
    Boost::thread
    Boost::program_options
    Boost::serialization
    nlohmann_json::nlohmann_json
)
target_include_directories(${TARGET_NAME} PRIVATE ${GLM_LIBRARY_INCLUDES})
target_link_libraries(${TARGET_NAME} PRIVATE glm::glm)

target_link_libraries(${TARGET_NAME} PUBLIC gtest_main)
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

# setup make target
set(RUN_TESTS_TARGET "run_${TARGET_NAME}")
add_custom_target(${RUN_TESTS_TARGET})
add_custom_command(
    TARGET ${RUN_TESTS_TARGET} 
    COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}"
    ARGS "--output-on-failure"
)
add_dependencies(${RUN_TESTS_TARGET} ${TARGET_NAME})


