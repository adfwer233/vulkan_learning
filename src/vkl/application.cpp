#include "application.hpp"
#include "vkl_model.hpp"

#include "system/simple_render_system.hpp"

Application::~Application() {
}

void Application::run() {

    SimpleRenderSystem renderSystem(device_, renderer_.getSwapChainRenderPass());

    std::vector<VklModel::Vertex> vertexData = {
            {{0.0, -0.5, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}},
            {{0.5,  0.5, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}},
            {{-0.5, 0.5, 0.0}, {0.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}}
    };

    std::vector<uint32_t> indices = {0, 1, 2};

    VklModel::BuilderFromImmediateData builder;
    builder.vertices = vertexData;
    builder.indices = indices;

    VklModel model(device_, builder);

    while (not window_.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = renderer_.beginFrame()) {
            renderer_.beginSwapChainRenderPass(commandBuffer);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              renderSystem.pipeline_->graphicsPipeline_);

            model.bind(commandBuffer);
            model.draw(commandBuffer);

            renderer_.endSwapChainRenderPass(commandBuffer);
            renderer_.endFrame();
        }
    }

    vkDeviceWaitIdle(device_.device());
}
