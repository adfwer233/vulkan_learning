#include "application.hpp"

#include "vkl/render_graph/render_graph.hpp"
#include "vkl/scene/vkl_scene.hpp"
#include "vkl/system/render_system/simple_render_system.hpp"

Application::~Application() {
}

void Application::run() {
    GLFWwindow *window = window_.getGLFWwindow();

    VklScene scene(device_, {0, 0, 10}, {0, 1, 0});

    VklModel::BuilderFromImmediateData builder;
    builder.vertices = {
        {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
        {{0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
        {{1.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    };

    scene.addObject(builder);

    RenderGraphDescriptor graphDescriptor;

    auto output_texture = graphDescriptor.attachment<RenderGraphTextureAttachment>("output_image");
    output_texture->isSwapChain = true;
    output_texture->format = VK_FORMAT_R8G8B8A8_SRGB;
    output_texture->width = 1024;
    output_texture->height = 1024;

    auto simple_render_pass = graphDescriptor.pass<RenderGraphRenderPass>("simple_render_pass");
    simple_render_pass->outTextureAttachmentDescriptors.push_back(output_texture);
    simple_render_pass->width = 1024;
    simple_render_pass->height = 1024;

    RenderGraph renderGraph(device_, swapChain_, &graphDescriptor, 3);

    renderGraph.createLayouts();

    renderGraph.createInstances();

    auto simple_render_pass_obj = renderGraph.getPass<RenderGraphRenderPass>("simple_render_pass");

    auto simple_render_system = simple_render_pass_obj->getRenderSystem<SimpleRenderSystem<>>(device_, "simple_render_system",
                                                                                            {{std::format("{}/first_triangle_shader.vert.spv", SHADER_DIR), VK_SHADER_STAGE_VERTEX_BIT},
                                                                                             {std::format("{}/first_triangle_shader.frag.spv", SHADER_DIR), VK_SHADER_STAGE_FRAGMENT_BIT}});

    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentFrame = 0;

    vkDeviceWaitIdle(device_.device());

    commandBuffers.resize(3);

    VkCommandBufferAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = device_.getCommandPool();
    allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    allocateInfo.pNext = nullptr;

    if (vkAllocateCommandBuffers(device_.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    // for (int i = 0; i < 2; i++) {
    //
    //     VkCommandBufferBeginInfo beginInfo{};
    //     beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //
    //     if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
    //         throw std::runtime_error("failed to begin recording command buffer!");
    //     }
    //
    //     vkEndCommandBuffer(commandBuffers[i]);
    // }

    while (not glfwWindowShouldClose(window)) {
        glfwPollEvents();

        auto result = swapChain_.acquireNextImage(&currentFrame);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = simple_render_pass_obj->renderPass;
        renderPassInfo.framebuffer = simple_render_pass_obj->instances[currentFrame]->framebuffer;

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {simple_render_pass_obj->descriptor_p->width, simple_render_pass_obj->descriptor_p->height};

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(simple_render_pass_obj->descriptor_p->width);
        viewport.height = static_cast<float>(simple_render_pass_obj->descriptor_p->height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, {1024, 1024}};
        vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);

        FrameInfo<VklModel> frameInfo{
            .frameIndex = static_cast<int>(currentFrame) % 2,
            .frameTime = 0,
            .commandBuffer = commandBuffers[currentFrame],
            .camera = scene.camera,
            .model = *scene.objects.front()->models.front(),
        };

        simple_render_system->renderObject(frameInfo);

        vkCmdEndRenderPass(commandBuffers[currentFrame]);

        vkEndCommandBuffer(commandBuffers[currentFrame]);

        std::vector<VkCommandBuffer> commandBufferToSubmit = {commandBuffers[currentFrame]};

        swapChain_.submitCommandBuffers(commandBufferToSubmit, &currentFrame);

        currentFrame = (currentFrame + 1) % 2;
    }
}