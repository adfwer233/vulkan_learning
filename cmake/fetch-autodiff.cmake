include(FetchContent)

FetchContent_Declare(
        autodiff
        GIT_REPOSITORY https://github.com/autodiff/autodiff.git
        GIT_TAG v1.1.2
)

FetchContent_MakeAvailable(autodiff)