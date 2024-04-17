set(LIB_NAME game_shared_lib)

set(FILES
    game/gamestate.cpp
    game/event.cpp
    game/gamelogic/object.cpp

    network/session.cpp

    utilities/config.cpp
    utilities/rng.cpp

    utilities/vector3.cpp
)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_include_directories(${LIB_NAME} PUBLIC ${BOOST_LIBRARY_INCLUDES})
target_link_libraries(${LIB_NAME}
    PRIVATE 
    Boost::asio
    Boost::filesystem
    Boost::thread
    Boost::program_options
    Boost::serialization
    Boost::dll
    nlohmann_json::nlohmann_json
)

target_include_directories(${LIB_NAME} PRIVATE ${GLM_LIBRARY_INCLUDES})
target_link_libraries(${LIB_NAME} PRIVATE glm::glm)

add_subdirectory(tests) # define shared unit tests
