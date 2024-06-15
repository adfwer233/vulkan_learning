include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        spirv_cross
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
        GIT_TAG origin/main
)

FetchContent_MakeAvailable(spirv_cross)