#pragma once
#include "glm/glm.hpp"

#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"

class VulkanDemoApplication {
private:
    GLFWwindow *window { nullptr };

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    VkInstance instance;

    void init_window() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void createInstance();

    void init_vulkan();

    void main_loop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void clean_up();

public:
    void run() {
        init_window();
        init_vulkan();
        main_loop();
        clean_up();
    }
};