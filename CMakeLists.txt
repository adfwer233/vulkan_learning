cmake_minimum_required(VERSION 3.26)

project(LearnVulkan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(fetch-glfw)
include(fetch-glm)
include(fetch-assimp)
include(fetch-stb)
include(fetch-imgui)

INCLUDE_DIRECTORIES(${stb_SOURCE_DIR})
INCLUDE_DIRECTORIES(${imgui_fetch_SOURCE_DIR})
INCLUDE_DIRECTORIES(${imgui_fetch_SOURCE_DIR}/backends)

# set vulkan SDK
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VK_SDK_PATH not defined!")
endif()
set(VK_SDK_PATH $ENV{VULKAN_SDK})
set(VK_SDK_INCLUDE ${VK_SDK_PATH}/Include)
set(VK_SDK_LIB ${VK_SDK_PATH}/Lib/vulkan-1.lib)

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

target_link_libraries(vkl glfw glm ${VK_SDK_LIB} assimp imgui)

message(STATUS "Build documentation open")
include(build-doxygen)
build_doxygen()

include(clang-format)