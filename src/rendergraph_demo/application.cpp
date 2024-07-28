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

    RenderGraph renderGraph(device_, &graphDescriptor, 2);

    renderGraph.createLayouts();

    renderGraph.createInstances();

    while (not glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}