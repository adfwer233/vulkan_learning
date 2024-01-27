#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

/**
 * \page windowPage GLFW window
 *
 * ## GLFW
 *
 * ## Encapsulation
 */

class vklWindow {
  private:
    uint32_t width_;
    uint32_t height_;

    GLFWwindow *window_{nullptr};

  public:
    vklWindow(int w, int h);

    GLFWwindow *getGLFWwindow() const {
        return this->window_;
    }

    [[nodiscard]] bool shouldClose() const {
        return glfwWindowShouldClose(this->window_);
    }

    VkExtent2D getExtent() {
        return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)};
    }
};