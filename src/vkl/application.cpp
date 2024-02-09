#include "application.hpp"

Application::~Application() {
}

void Application::run() {
    while (not window_.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = renderer_.beginFrame()) {
            renderer_.beginSwapChainRenderPass(commandBuffer);
            renderer_.beginFrame();
            renderer_.endSwapChainRenderPass(commandBuffer);
            renderer_.endFrame();
        }
    }
}
