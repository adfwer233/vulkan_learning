#include "application.hpp"

#include "vkl/render_graph/render_graph.hpp"

Application::~Application() {
}

void Application::run() {
    GLFWwindow *window = window_.getGLFWwindow();

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

    RenderGraph renderGraph(device_, swapChain_, &graphDescriptor, 2);

    renderGraph.createLayouts();

    renderGraph.createInstances();

    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentFrame = 0;

    vkDeviceWaitIdle(device_.device());

    commandBuffers.resize(2);

    VkCommandBufferAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = device_.getCommandPool();
    allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    allocateInfo.pNext = nullptr;

    if (vkAllocateCommandBuffers(device_.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (int i = 0; i < 2; i++) {

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        vkEndCommandBuffer(commandBuffers[i]);
    }

    while (not glfwWindowShouldClose(window)) {
        glfwPollEvents();

        auto result = swapChain_.acquireNextImage(&currentFrame);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        std::vector<VkCommandBuffer> commandBufferToSubmit = {commandBuffers[0]};

        swapChain_.submitCommandBuffers(commandBufferToSubmit, &currentFrame);

        currentFrame = (currentFrame + 1) % 2;
    }
}