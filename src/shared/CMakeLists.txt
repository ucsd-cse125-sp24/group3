set(LIB_NAME game_shared_lib)

set(FILES
    game/gamestate.cpp

    network/packet.cpp

    utilities/rng.cpp
    utilities/test.cpp
)

set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options asio date_time)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_PROGRESS TRUE
    GIT_TAG boost-1.82.0
)
FetchContent_MakeAvailable(Boost)

INCLUDE_DIRECTORIES(${BOOST_LIBRARY_INCLUDES})
include_directories(asio INTERFACE ${boost_asio_SOURCE_DIR}/include)

add_library(${LIB_NAME} STATIC ${FILES})
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_include_directories(${LIB_NAME} PRIVATE ${BOOST_LIBRARY_INCLUDES})

target_link_libraries(${LIB_NAME}
  PRIVATE 
  Boost::asio
  Boost::filesystem
  Boost::thread
  Boost::program_options
)