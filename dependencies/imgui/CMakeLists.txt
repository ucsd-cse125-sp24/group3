include(FetchContent)

FetchContent_Declare(imgui 
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.4.zip
    URL_HASH SHA256=c6b91ff89ecdef2ef568a2eda87d3d9f5421c619a2cdfa9836752ee84aabaf2b
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_MakeAvailable(imgui)

# ImGui
set(imgui-directory ${CMAKE_BINARY_DIR}/_deps/imgui-src/ PARENT_SCOPE)
set(imgui-directory ${CMAKE_BINARY_DIR}/_deps/imgui-src/)

set(imgui-source 
${imgui-directory}/imconfig.h
	${imgui-directory}/imgui.h
	${imgui-directory}/imgui.cpp
	${imgui-directory}/imgui_draw.cpp
	${imgui-directory}/imgui_internal.h
	${imgui-directory}/imgui_widgets.cpp
	${imgui-directory}/imstb_rectpack.h
	${imgui-directory}/imstb_textedit.h
	${imgui-directory}/imstb_truetype.h
    ${imgui-directory}/imgui_tables.cpp
	${imgui-directory}/imgui_demo.cpp
	${imgui-directory}/backends/imgui_impl_glfw.cpp
	${imgui-directory}/backends/imgui_impl_opengl3.cpp
	PARENT_SCOPE
)