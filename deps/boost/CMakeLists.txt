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
