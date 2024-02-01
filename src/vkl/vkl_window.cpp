#include "vkl_window.hpp"

#include <stdexcept>

VklWindow::VklWindow(int width, int height) : width_(width), height_(height) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    this->window_ = glfwCreateWindow(width, height, "Learn Vulkan", nullptr, nullptr);
}

void VklWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to craete window surface");
    }
}
