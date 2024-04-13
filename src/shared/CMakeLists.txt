set(LIB_NAME game_shared_lib)

set(FILES
    game/gamestate.cpp

    network/packet.cpp

    utilities/rng.cpp
)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})

add_subdirectory(tests) # define shared unit tests