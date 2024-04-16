set(TARGET_NAME server)

set(FILES
    main.cpp
    server.cpp
)

add_executable(${TARGET_NAME} ${FILES})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_link_libraries(${TARGET_NAME} PRIVATE game_shared_lib)

target_include_directories(${TARGET_NAME} PRIVATE ${GLM_LIBRARY_INCLUDES})
target_link_libraries(${TARGET_NAME} PRIVATE glm::glm)