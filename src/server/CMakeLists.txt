set(TARGET_NAME server)
set(LIB_NAME game_server_lib)

set(FILES
    server.cpp
)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})

add_executable(${TARGET_NAME} main.cpp)

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})

target_include_directories(${LIB_NAME} PRIVATE ${GLM_LIBRARY_INCLUDES})
target_link_libraries(${LIB_NAME} PRIVATE glm::glm)

target_link_libraries(${TARGET_NAME} PRIVATE ${LIB_NAME} game_shared_lib)

add_subdirectory(tests) # define server unit tests
