#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "vector"
#include "vkl_window.hpp"

/**
 * \page devicePage Vulkan Device
 *
 * ## Concepts
 *  - Instance
 *      - Instance connects application with vulkan library.
 *  - Extension
 *      - i.e. additional features, such as enable validation layer
 *
 *  ## Initialization
 *
 *  ### Create Instance
 *
 *  When we want to create a "device" instance, the first step is creating a vulkan instance.
 *
 *  To create a vulkan instance, i.e. `VkInstance`, we should create a `VkInstanceCreateInfo` struct and fill it,
 * telling vulkan engine some messages
 *
 *  - Basic application information
 *  - Required extensions such as validation layer
 *  - Debug messenger we are going to use
 */

/**
 * @brief Vulkan device
 */
class vklDevice {
  private:
    const bool enableValidationLayers = true;

    VkInstance instance_;                     /**< Vulkan Instance */
    VkDebugUtilsMessengerEXT debugMessenger_; /** Vulkan Debug Messenger */
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    vklWindow &window_;         /** Pointer to encapsulated vulkan window */
    VkCommandPool commandPool_; /** Command Pool */

    VkDevice device_;       /** Logical Device */
    VkSurfaceKHR surface_;  /** Vulkan Surface */
    VkQueue graphicsQueue_; /** Graphic Queue  */
    VkQueue presentQueue_;  /** Present Queue  */

    /**
     * @brief Required Validation Layers
     */
    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    /**
     * @brief Required Device Extensions
     */
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    /** Auxiliary functions */

    /**
     * @brief create vulkan instance
     */
    void createInstance();
    void setupDebugMessenger();

    std::vector<const char *> getRequiredExtensions() const;

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT createInfo);

  public:
    explicit vklDevice(vklWindow &window);
    ~vklDevice();

    // Not copyable or movable
    vklDevice(const vklDevice &) = delete;
    vklDevice &operator=(const vklDevice &) = delete;
    vklDevice(vklDevice &&) = delete;
    vklDevice &operator=(vklDevice &&) = delete;

    VkCommandPool getCommandPool() {
        return commandPool_;
    }
    VkDevice device() {
        return device_;
    }
    VkSurfaceKHR surface() {
        return surface_;
    }
    VkQueue graphicsQueue() {
        return graphicsQueue_;
    }
    VkQueue presentQueue() {
        return presentQueue_;
    }
};