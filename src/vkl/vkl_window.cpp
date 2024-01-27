#include "vkl_window.hpp"

vklWindow::vklWindow(int width, int height) : width_(width), height_(height) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    this->window_ = glfwCreateWindow(width, height, "Learn Vulkan", nullptr, nullptr);
}
