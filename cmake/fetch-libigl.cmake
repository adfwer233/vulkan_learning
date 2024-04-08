if(TARGET igl::core)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
        libigl
        GIT_REPOSITORY https://github.com/adfwer233/libigl.git
        GIT_TAG origin/personal_patch
)
FetchContent_MakeAvailable(libigl)