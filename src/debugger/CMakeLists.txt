set(TARGET_NAME debugger)

set(FILES
	main.cpp
	Debugger.cpp
)

add_executable(${TARGET_NAME} ${FILES})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_link_libraries(${TARGET_NAME} PRIVATE game_shared_lib)