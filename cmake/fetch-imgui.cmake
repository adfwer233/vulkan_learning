include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        imgui_fetch
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG origin/docking
)

FetchContent_MakeAvailable(imgui_fetch)