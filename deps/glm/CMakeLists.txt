cmake_minimum_required(VERSION 3.11)

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

INCLUDE_DIRECTORIES(${GLM_LIBRARY_INCLUDES})
include_directories(glm INTERFACE ${GLM_SOURCE_DIR}/include)
