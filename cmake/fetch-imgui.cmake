include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        imgui_fetch
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
)

FetchContent_MakeAvailable(imgui_fetch)