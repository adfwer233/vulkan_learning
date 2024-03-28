include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        tinyobjloader
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
        GIT_TAG origin/release
)

FetchContent_Populate(tinyobjloader)