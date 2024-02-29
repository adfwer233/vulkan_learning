cmake_minimum_required(VERSION 3.26)

project(LearnVulkan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(fetch-glfw)
include(fetch-glm)

# set vulkan SDK
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VK_SDK_PATH not defined!")
endif()
set(VK_SDK_PATH $ENV{VULKAN_SDK})
set(VK_SDK_INCLUDE ${VK_SDK_PATH}/Include)
set(VK_SDK_LIB ${VK_SDK_PATH}/Lib/vulkan-1.lib)

add_subdirectory(src/vkl)

target_link_libraries(vkl glfw glm ${VK_SDK_LIB})

message(STATUS "Build documentation open")
include(build-doxygen)
build_doxygen()

include(clang-format)