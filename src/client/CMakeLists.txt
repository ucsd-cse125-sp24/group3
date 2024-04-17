set(TARGET_NAME client)
set(LIB_NAME game_client_lib)

set(FILES
    client.cpp
    lobbyfinder.cpp
)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_include_directories(${LIB_NAME} PRIVATE ${BOOST_LIBRARY_INCLUDES})
target_link_libraries(${LIB_NAME} 
    PRIVATE
    game_shared_lib
    Boost::asio
    Boost::filesystem
    Boost::thread
    Boost::program_options
    Boost::serialization
    nlohmann_json::nlohmann_json
)
target_include_directories(${LIB_NAME} PRIVATE ${GLM_LIBRARY_INCLUDES})
target_link_libraries(${LIB_NAME} PRIVATE glm::glm)

add_executable(${TARGET_NAME} main.cpp)

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_link_libraries(${TARGET_NAME} PRIVATE game_shared_lib ${LIB_NAME})

target_include_directories(${TARGET_NAME} PRIVATE ${GLM_LIBRARY_INCLUDES})
target_link_libraries(${TARGET_NAME} PRIVATE glm::glm)

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


add_subdirectory(tests) # define client unit tests
