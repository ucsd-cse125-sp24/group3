set(TARGET_NAME server)
set(LIB_NAME game_server_lib)

set(FILES
    lobbybroadcaster.cpp
    server.cpp
    game/collider.cpp
    game/creature.cpp
    game/item.cpp
    game/solidsurface.cpp
    game/object.cpp
    game/servergamestate.cpp
    game/objectmanager.cpp
    game/player.cpp
    game/enemy.cpp
    game/gridcell.cpp
    game/grid.cpp
    game/trap.cpp
    game/spiketrap.cpp
    game/fireballtrap.cpp
    game/arrowtrap.cpp
    game/projectile.cpp
    game/floorspike.cpp
    game/fakewall.cpp
    game/potion.cpp
    game/mazegenerator.cpp
    game/teleportertrap.cpp
    game/exit.cpp
    game/orb.cpp
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

add_subdirectory(tests) # define server unit tests