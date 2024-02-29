include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
)

FetchContent_Populate(stb)