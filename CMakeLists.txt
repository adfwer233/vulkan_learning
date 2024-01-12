cmake_minimum_required(VERSION 3.26)

project(LearnVulkan)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(fetch-glfw)
include(fetch-glm)

add_subdirectory(src/demo)

target_link_libraries(demo glfw glm)

message(STATUS "Build documentation open")
include(build-doxygen)
build_doxygen()

include(clang-format)