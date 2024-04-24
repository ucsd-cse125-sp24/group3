set(TARGET_NAME debugger)

set(FILES
	main.cpp
	debugger.cpp
	../server/game/servergamestate.cpp
	../server/game/object.cpp
	../server/game/objectmanager.cpp
)

add_executable(${TARGET_NAME} ${FILES})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_link_libraries(${TARGET_NAME} PRIVATE game_shared_lib nlohmann_json::nlohmann_json)

target_include_directories(${TARGET_NAME} PRIVATE ${GLM_LIBRARY_INCLUDES})
target_link_libraries(${TARGET_NAME} PRIVATE glm::glm)