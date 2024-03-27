include(FetchContent)

FetchContent_Declare(
        boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
)

FetchContent_MakeAvailable(boost)