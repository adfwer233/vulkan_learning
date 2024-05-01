cmake_minimum_required(VERSION 3.26)

project(LearnVulkan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CUDA_ENABLE OFF CACHE BOOL "Whether enable cuda kernel")

if (CUDA_ENABLE)
    if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        set(CMAKE_CUDA_ARCHITECTURES 50)
    endif()

    enable_language(CUDA)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(fetch-glfw)
include(fetch-glm)
include(fetch-assimp)
include(fetch-boost)
include(fetch-eigen)
include(fetch-stb)
include(fetch-imgui)
include(fetch-jsoncpp)
include(fetch-libigl)
include(fetch-googletest)
include(fetch-tinyobjloader)
include(fetch-autodiff)
include(fetch-random)
include(fetch-rootfinder)
include(fetch-svgpp)

igl_include(copyleft core)

INCLUDE_DIRECTORIES(${stb_SOURCE_DIR})
INCLUDE_DIRECTORIES(${svgpp_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${svgpp_SOURCE_DIR}/third_party)
INCLUDE_DIRECTORIES(${root_finder_SOURCE_DIR}/root_finder/include)
INCLUDE_DIRECTORIES(${tinyobjloader_SOURCE_DIR})
INCLUDE_DIRECTORIES(${random_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${imgui_fetch_SOURCE_DIR})
INCLUDE_DIRECTORIES(${imgui_fetch_SOURCE_DIR}/backends)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

message(STATUS ${gtest_SOURCE_DIR})
message(STATUS ${Boost_INCLUDE_DIRS})

# set vulkan SDK
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VK_SDK_PATH not defined!")
endif()

set(VK_SDK_PATH $ENV{VULKAN_SDK})
set(VK_SDK_INCLUDE ${VK_SDK_PATH}/Include)
set(VK_SDK_LIB ${VK_SDK_PATH}/Lib/vulkan-1.lib)

if (UNIX)
    message(STATUS "Set Linux Vulkan SDK Path")
    set(VK_SDK_LIB ${VK_SDK_PATH}/lib/libvulkan.so.1)
endif ()

add_library(imgui
        ${imgui_fetch_SOURCE_DIR}/imgui.cpp
        ${imgui_fetch_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_fetch_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_fetch_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_fetch_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_fetch_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_fetch_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)
target_include_directories(imgui PUBLIC ${VK_SDK_INCLUDE})
target_include_directories(imgui PUBLIC ${GLFW_SOURCE_DIR}/include)

add_subdirectory(src/vkl)
add_subdirectory(src/ray_tracer)
add_subdirectory(src/demo)
add_subdirectory(src/particle)
add_subdirectory(src/geometry_processing)
add_subdirectory(src/bezier_curve)
add_subdirectory(src/geometry)
add_subdirectory(src/simulation)
add_subdirectory(test)

find_package(OpenMP)
target_link_libraries(vkl glfw glm ${VK_SDK_LIB} assimp imgui OpenMP::OpenMP_CXX)

if(MSVC)
    add_compile_options("/source-charset:utf-8")
endif()

message(STATUS "Build documentation open")
include(build-doxygen)
build_doxygen()

include(clang-format)

add_custom_target(BUILD_ALL)
add_dependencies(BUILD_ALL vkl demo particle geometry test)
