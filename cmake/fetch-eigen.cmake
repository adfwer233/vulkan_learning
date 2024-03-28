include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
)

FetchContent_MakeAvailable(eigen)