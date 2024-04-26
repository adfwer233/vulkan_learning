include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        svgpp
        GIT_REPOSITORY https://github.com/svgpp/svgpp.git
)

FetchContent_Populate(svgpp)