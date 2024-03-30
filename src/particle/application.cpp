#include "application.hpp"
#include "vkl/core/vkl_descriptor.hpp"
#include "vkl/scene/vkl_object.hpp"

#include "particle/system/particle_render_system.hpp"
#include "particle/system/particle_simulation_system.hpp"
#include "vkl/system/simple_render_system.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "particle/particle.hpp"

#include <functional>
#include <random>

#ifndef PARTICLE_SHADER_DIR
#define PARTICLE_SHADER_DIR "./shader/"
#endif

using VklParticleModel = VklModelTemplate<Particle, TriangleIndex, VklBox2D>;

Application::~Application() {
}

void Application::run() {

    const size_t particle_number = 2048000;

    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> radiusDist(0.5f, 1.0f);

    // Initial particle positions on a circle
    std::vector<Particle> particles(particle_number);
    for (auto &particle : particles) {
        float r = 0.25f * sqrt(radiusDist(rndEngine));
        float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
        float x = r * cos(theta) * HEIGHT / WIDTH;
        float y = r * sin(theta);
        particle.position = glm::vec2(x, y);
        particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.025f;
        particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
    }

    VklParticleModel::BuilderFromImmediateData builder;
    builder.vertices = particles;

    VklParticleModel model(device_, builder);

    auto globalSetLayout = VklDescriptorSetLayout::Builder(device_).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();

    auto globalPool = VklDescriptorPool::Builder(device_).setMaxSets(VklSwapChain::MAX_FRAMES_IN_FLIGHT * 200).build();

    model.allocDescriptorSets(*globalSetLayout, *globalPool);

    /** set camera */

    Camera camera({0, 0, 1}, {0, 1, 0});

    GLFWwindow *window = window_.getGLFWwindow();

    float deltaTime = 0, lastFrame = 0;

    ParticleRenderSystem renderSystem(device_, renderer_.getSwapChainRenderPass(),
                                      globalSetLayout->getDescriptorSetLayout(),
                                      std::format("{}/particle.vert.spv", PARTICLE_SHADER_DIR),
                                      std::format("{}/particle.frag.spv", PARTICLE_SHADER_DIR));

    ParticleSimulationSystem computeSystem(device_, model, particle_number);

    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imguiPool;
    vkCreateDescriptorPool(device_.device(), &pool_info, nullptr, &imguiPool);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    device_.fillImGuiInitInfo(init_info);
    init_info.DescriptorPool = imguiPool;
    init_info.RenderPass = renderer_.getSwapChainRenderPass();
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 2;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);

    bool show_demo_window = true;

    int triangle_num = 0;

    while (not window_.shouldClose()) {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (true) {
            int frameIndex = renderer_.getFrameIndex();

            ImGui::Begin("Messages");
            ImGui::SeparatorText("Scene Information");
            ImGui::LabelText("# Particles", "%d", particle_number);
            ImGui::LabelText(
                "Camera Position",
                std::format("{:.3f}, {:.3f}, {:.3f}", camera.position.x, camera.position.y, camera.position.z).c_str());
            ImGui::SeparatorText("Performance");
            ImGui::LabelText("FPS", std::format("{:.3f}", 1 / deltaTime).c_str());
            ImGui::End();

            computeSystem.computeSubmission(frameIndex, deltaTime);
            renderer_.setSemaphoreToWait(computeSystem.computeFinishedSemaphores[frameIndex]);

            auto commandBuffer = renderer_.beginFrame();

            FrameInfo<VklParticleModel> frameInfo{
                frameIndex, currentFrame, commandBuffer, camera, &model.descriptorSets[frameIndex], model};

            renderer_.beginSwapChainRenderPass(commandBuffer);
            renderSystem.renderObject(frameInfo);

            /* ImGui Rendering */
            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

            renderer_.endSwapChainRenderPass(commandBuffer);

            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }

            std::vector<VkCommandBuffer> commandBuffers{commandBuffer};

            auto result = renderer_.swapChain_->submitCommandBuffers(commandBuffers, &renderer_.currentImageIndex, renderer_.getSemaphoreToWait());

            renderer_.endFrame();
        }
    }

    vkDeviceWaitIdle(device_.device());

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    /** destroy imgui descriptor pool*/
    vkDestroyDescriptorPool(device_.device(), imguiPool, nullptr);
}
