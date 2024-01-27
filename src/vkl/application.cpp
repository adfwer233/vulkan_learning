#include "application.hpp"

Application::~Application() {
}

void Application::run() {
    while (window_.shouldClose()) {
        glfwPollEvents();
    }
}
