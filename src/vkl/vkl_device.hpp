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
 *
 *  ### Physical Device and Logical Device
 *
 *  **Queue Families**: In vulkan, almost every operation should be done through a queue and there are different type
 *  of queues. Each family of queues allows only a subset of commands. Hence we should query if our devices support
 *  queue families we want to use.
 *
 *  \note Pick Physical Device
 *
 *  In this step, we traverse all physical device and pick one satisfying our requirement.
 *
 *  \note Create Logical Device
 *
 *  Firstly, we get all queue family properties. Then find the indice of graphics queue and present queue.
 *  With these indices, we can fill a create info struct `VkDeviceCreateInfo` and invoke `vkCreateDevice` to create
 *  logical device.
 *
 *
 */

/**
 * @brire Queue Family Indices
 */
struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() {
        return graphicsFamilyHasValue && presentFamilyHasValue;
    }
};

/**
 * @brief Swap Chain Support Details
 */
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

/**
 * @brief Vulkan device
 */
class VklDevice {
  private:
    const bool enableValidationLayers = true;

    VkInstance instance_;                     /**< Vulkan Instance */
    VkDebugUtilsMessengerEXT debugMessenger_; /** Vulkan Debug Messenger */
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VklWindow &window_;         /** Pointer to encapsulated vulkan window */
    VkCommandPool commandPool_; /** Command Pool */

    VkDevice device_;       /** Logical Device */
    VkSurfaceKHR surface_;  /** Vulkan Surface */
    VkQueue graphicsQueue_; /** Graphic Queue  */
    VkQueue presentQueue_;  /** Present Queue  */

    VkPhysicalDeviceProperties properties_; /** Physical device properties */

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
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();

    [[nodiscard]] std::vector<const char *> getRequiredExtensions() const;

    bool isDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  public:
    explicit VklDevice(VklWindow &window);
    ~VklDevice();

    // Not copyable or movable
    VklDevice(const VklDevice &) = delete;
    VklDevice &operator=(const VklDevice &) = delete;
    VklDevice(VklDevice &&) = delete;
    VklDevice &operator=(VklDevice &&) = delete;

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

    SwapChainSupportDetails getSwapChainSupport() {
        return querySwapChainSupport(physicalDevice_);
    }

    QueueFamilyIndices findPhysicalQueueFamilies() {
        return findQueueFamilies(physicalDevice_);
    }
};
