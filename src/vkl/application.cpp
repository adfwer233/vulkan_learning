#include "application.hpp"

#include "system/simple_render_system.hpp"

Application::~Application() {
}

void Application::run() {

    SimpleRenderSystem renderSystem(device_, renderer_.getSwapChainRenderPass());

    while (not window_.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = renderer_.beginFrame()) {
            renderer_.beginSwapChainRenderPass(commandBuffer);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              renderSystem.pipeline_->graphicsPipeline_);
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);

            renderer_.endSwapChainRenderPass(commandBuffer);
            renderer_.endFrame();
        }
    }

    vkDeviceWaitIdle(device_.device());
}
