#include "application.hpp"

Application::~Application() {
}

void Application::run() {
    GLFWwindow *window = window_.getGLFWwindow();

    while (not glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}