include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        root_finder
        GIT_REPOSITORY https://github.com/ZhepeiWang/Root-Finder.git
)

FetchContent_Populate(root_finder)