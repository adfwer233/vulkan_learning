include(FetchContent)

FetchContent_Declare(
        autodiff
        GIT_REPOSITORY https://github.com/autodiff/autodiff.git
        GIT_TAG origin/main
)

FetchContent_MakeAvailable(autodiff)