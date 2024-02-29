#include "application.hpp"
#include "vkl_descriptor.hpp"
#include "vkl_model.hpp"

#include "system/simple_render_system.hpp"

Application::~Application() {
}

void Application::run() {

    /** set uniform buffers */

    std::vector<std::unique_ptr<VklBuffer>> uniformBuffers(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uniformBuffers.size(); i++) {
        uniformBuffers[i] = std::make_unique<VklBuffer>(
            device_, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uniformBuffers[i]->map();
    }

    auto globalSetLayout = VklDescriptorSetLayout::Builder(device_)
                               .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                               .build();

    auto globalPool = VklDescriptorPool::Builder(device_)
                          .setMaxSets(VklSwapChain::MAX_FRAMES_IN_FLIGHT)
                          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VklSwapChain::MAX_FRAMES_IN_FLIGHT)
                          .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uniformBuffers[i]->descriptorInfo();
        VklDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
    }

    /** set camera */

    Camera camera({0, 0, 3}, {0, 1, 0});

    /** render system */

    SimpleRenderSystem renderSystem(device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());

    /** tmp model data */
    std::vector<VklModel::Vertex> vertexData = {{{0.0, -0.5, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}},
                                                {{0.5, 0.5, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}},
                                                {{-0.5, 0.5, 0.0}, {0.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}}};

    std::vector<uint32_t> indices = {0, 1, 2};

    VklModel::BuilderFromImmediateData builder;
    builder.vertices = vertexData;
    builder.indices = indices;

    VklModel model(device_, builder);

    while (not window_.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = renderer_.beginFrame()) {
            int frameIndex = renderer_.getFrameIndex();

            renderer_.beginSwapChainRenderPass(commandBuffer);

            GlobalUbo ubo{};

            ubo.view = camera.get_view_transformation();
            ubo.proj = camera.get_proj_transformation();
            ubo.model = glm::mat4(1.0f);

            uniformBuffers[frameIndex]->writeToBuffer(&ubo);
            uniformBuffers[frameIndex]->flush();

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              renderSystem.pipeline_->graphicsPipeline_);

            vkCmdBindDescriptorSets(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    renderSystem.pipelineLayout_,
                    0,
                    1,
                    globalDescriptorSets.data(),
                    0,
                    nullptr);

            model.bind(commandBuffer);
            model.draw(commandBuffer);

            renderer_.endSwapChainRenderPass(commandBuffer);
            renderer_.endFrame();
        }
    }

    vkDeviceWaitIdle(device_.device());
}
