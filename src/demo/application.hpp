#pragma once

#include <vector>
#include <optional>
#include "glm/ext.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

class VulkanDemoApplication {
private:
    GLFWwindow *window { nullptr };

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const std::vector<const char*> validationLayers {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool enableValidationLayers { false };
#else
    const bool enableValidationLayers { true };
#endif

    VkInstance instance;
    VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };
    VkDevice device;
    VkQueue graphicsQueue;

    VkSurfaceKHR surface;

    void init_window() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    bool checkValidationLayerSupport();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSurface();

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