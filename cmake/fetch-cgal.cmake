include(FetchContent)

FetchContent_Declare(
        CGAL
        GIT_REPOSITORY https://github.com/CGAL/cgal.git
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
)

FetchContent_MakeAvailable(CGAL)

set(CGAL_DIR "${CMAKE_BINARY_DIR}/_deps/cgal-src" CACHE PATH "path to checkout CGAL" FORCE)

find_package(CGAL REQUIRED)