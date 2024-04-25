include(FetchContent)

FetchContent_Declare(imgui 
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.4.zip
)

FetchContent_MakeAvailable(imgui)

# ImGui
set(imgui-directory ${CMAKE_BINARY_DIR}/_deps/imgui-src/ PARENT_SCOPE)
set(imgui-directory ${CMAKE_BINARY_DIR}/_deps/imgui-src/)

set(IMGUI_INCLUDE_DIRS
    ${imgui-directory}
    ${imgui-directory}/backends
    PARENT_SCOPE
)

set(IMGUI_SOURCES
	${imgui-directory}/imgui.cpp
	${imgui-directory}/imgui_draw.cpp
	${imgui-directory}/imgui_widgets.cpp
    ${imgui-directory}/imgui_tables.cpp
	${imgui-directory}/imgui_demo.cpp
	${imgui-directory}/backends/imgui_impl_glfw.cpp
	${imgui-directory}/backends/imgui_impl_opengl3.cpp
    ${imgui-directory}misc/cpp/imgui_stdlib.cpp
	PARENT_SCOPE
)