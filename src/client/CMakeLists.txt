set(TARGET_NAME client)

set(FILES
    main.cpp
    client.cpp
)

add_executable(${TARGET_NAME} ${FILES})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_link_libraries(${TARGET_NAME} PRIVATE game_shared_lib)

target_include_directories(${TARGET_NAME} PRIVATE ${BOOST_LIBRARY_INCLUDES})
target_link_libraries(${TARGET_NAME} 
  PRIVATE 
  Boost::asio
  Boost::filesystem
  Boost::thread
  Boost::program_options
)