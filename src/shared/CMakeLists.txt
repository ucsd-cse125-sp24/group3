set(LIB_NAME game_shared_lib)

set(FILES
    game/gamestate.cpp

    network/packet.cpp
    network/session.cpp

    utilities/config.cpp
    utilities/rng.cpp
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