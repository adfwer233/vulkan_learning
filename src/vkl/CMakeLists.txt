project(vkl VERSION 1.0 DESCRIPTION "VulkanLearning" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILE main.cpp
        vkl_window.cpp
        application.cpp
        vkl_device.cpp
        vkl_swap_chain.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILE})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${VK_SDK_INCLUDE})


target_link_libraries(${PROJECT_NAME} glm glfw)