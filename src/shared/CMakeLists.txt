set(LIB_NAME game_shared_lib)

set(FILES
    game/GameState.cpp

    game/GameLogic/Object.cpp

    network/packet.cpp

    utilities/rng.cpp

    utilities/vector3.cpp
)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})

target_include_directories(${LIB_NAME} PRIVATE ${GLM_LIBRARY_INCLUDES})
target_link_libraries(${LIB_NAME} PRIVATE glm::glm)
add_subdirectory(tests) # define shared unit tests
