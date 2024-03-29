#include "application.hpp"
#include "vkl/utils/vkl_box.hpp"
#include "vkl/utils/vkl_box_model.hpp"
#include "vkl/core/vkl_descriptor.hpp"
#include "vkl/core/vkl_image.hpp"
#include "vkl/scene/vkl_object.hpp"
#include "vkl/scene/vkl_scene.hpp"

#include "demo/utils/controller.hpp"
#include "vkl/system/line_render_system.hpp"
#include "vkl/system/simple_render_system.hpp"
#include "vkl/system/simple_wireframe_render_system.hpp"
#include "vkl/system/normal_render_system.hpp"

#include "vkl/system/base_compute_system.hpp"
#include "vkl/system/path_tracing_compute_system.hpp"

#include "vkl/core/vkl_offscreen_renderer.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <functional>
#include <random>

#include "ui/ui_manager.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

static ImGui_ImplVulkanH_Window g_MainWindowData;

Application::~Application() {
}

void Application::run() {

    VklScene scene(device_, {0, 0, 10}, {0, 1, 0});

    /** set uniform buffers */

    std::vector<std::unique_ptr<VklBuffer>> uniformBuffers(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uniformBuffers.size(); i++) {
        uniformBuffers[i] = std::make_unique<VklBuffer>(
            device_, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uniformBuffers[i]->map();
    }

    auto globalSetLayout = VklDescriptorSetLayout::Builder(device_)
                               .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                               .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                               .build();

    auto globalPool = VklDescriptorPool::Builder(device_)
                          .setMaxSets(VklSwapChain::MAX_FRAMES_IN_FLIGHT * 200)
                          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VklSwapChain::MAX_FRAMES_IN_FLIGHT * 200)
                          .build();

    auto boxModel = VklBoxModel3D(device_, getStandardBox3D());
    boxModel.allocDescriptorSets(*globalSetLayout, *globalPool);

    KeyboardCameraController::setCamera(scene.camera);

    GLFWwindow *window = window_.getGLFWwindow();

    glfwSetCursorPosCallback(window, KeyboardCameraController::mouse_callback);
    glfwSetScrollCallback(window, KeyboardCameraController::scroll_callback);
    glfwSetMouseButtonCallback(window, KeyboardCameraController::mouse_button_callback);

    /** render system */
    SimpleRenderSystem<VklModel::vertex_type> renderSystem(device_, offscreenRenderer_.getSwapChainRenderPass(),
                                                           globalSetLayout->getDescriptorSetLayout());

    SimpleRenderSystem<VklModel::vertex_type> rawRenderSystem(
        device_, offscreenRenderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/simple_shader.vert.spv", SHADER_DIR),
        std::format("{}/point_light_shader.frag.spv", SHADER_DIR));

    SimpleWireFrameRenderSystem<VklModel::vertex_type> wireFrameRenderSystem(
        device_, offscreenRenderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/simple_shader.vert.spv", SHADER_DIR),
        std::format("{}/point_light_shader.frag.spv", SHADER_DIR));

    LineRenderSystem<VklBoxModel3D::vertex_type> lineRenderSystem(
        device_, offscreenRenderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/line_shader.vert.spv", SHADER_DIR), std::format("{}/line_shader.frag.spv", SHADER_DIR));

    SimpleRenderSystem<VklModel::vertex_type> colorRenderSystem(
        device_, offscreenRenderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/simple_shader.vert.spv", SHADER_DIR),
        std::format("{}/simple_color_shader.frag.spv", SHADER_DIR));

    SimpleRenderSystem<VklModel::vertex_type> backGroundRenderSystem(
        device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/simple_shader.vert.spv", SHADER_DIR),
        std::format("{}/path_tracing_post_shader.frag.spv", SHADER_DIR));

    NormalRenderSystem<VklModel::vertex_type> normalRenderSystem(
        device_, offscreenRenderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/normal_shader.vert.spv", SHADER_DIR),
        std::format("{}/line_shader.frag.spv", SHADER_DIR),
        std::format("{}/normal_generation.geom.spv", SHADER_DIR));

    float deltaTime = 0, lastFrame = 0;

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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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

    UIManager uiManager(device_, scene);

    VklTexture *renderRes = new VklTexture(device_, 1024, 1024, 4);

    device_.transitionImageLayout(renderRes->image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    uiManager.renderResultTexture = renderRes;
    uiManager.offscreenImageViews = offscreenRenderer_.getImageView();
    uiManager.offscreenSampler = offscreenRenderer_.imageSampler;

    KeyboardCameraController::set_scene(scene);
    KeyboardCameraController::setUIManager(uiManager);

    KeyboardCameraController::actionCallBack = [&]() {
        if (uiManager.pathTracingComputeSystem_ != nullptr) {
            uiManager.pathTracingComputeSystem_->computeModel_.ubo.currentSample = 0;
        }
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distrib(0, 1.0);

    while (not window_.shouldClose()) {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        uiManager.deltaTime = deltaTime;

        KeyboardCameraController::processInput(window_.getGLFWwindow(), deltaTime);

        int frameIndex = renderer_.getFrameIndex();
        uiManager.frameIndex = frameIndex;

        uiManager.renderImgui();

        if (uiManager.renderMode == 3) {

            auto commandBuffer = renderer_.beginFrame();

            auto targetTexture = uiManager.pathTracingComputeModel_->getTargetTexture();
            auto accumulationTexture = uiManager.pathTracingComputeModel_->getAccumulationTexture();

            uiManager.pathTracingComputeSystem_->computeModel_.ubo.cameraZoom = scene.camera.zoom;
            uiManager.pathTracingComputeSystem_->computeModel_.ubo.cameraPosition = scene.camera.position;
            uiManager.pathTracingComputeSystem_->computeModel_.ubo.cameraUp = scene.camera.camera_up_axis;
            uiManager.pathTracingComputeSystem_->computeModel_.ubo.cameraFront = scene.camera.camera_front;
            uiManager.pathTracingComputeSystem_->computeModel_.ubo.currentSample += 1;
            uiManager.pathTracingComputeSystem_->computeModel_.ubo.rand1 = distrib(gen);
            uiManager.pathTracingComputeSystem_->computeModel_.ubo.rand2 = distrib(gen);
            uiManager.pathTracingComputeSystem_->updateUniformBuffer(frameIndex);
            uiManager.pathTracingComputeSystem_->recordCommandBuffer(commandBuffer, targetTexture, accumulationTexture,
                                                                     renderRes->image_, frameIndex);

            renderer_.beginSwapChainRenderPass(commandBuffer);

            /* ImGui Rendering */
            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

            renderer_.endSwapChainRenderPass(commandBuffer);

            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }

            std::vector<VkCommandBuffer> commandBuffers{commandBuffer};

            auto result = renderer_.swapChain_->submitCommandBuffers(commandBuffers, &renderer_.currentImageIndex);

            renderer_.endFrame();

        } else {

            auto commandBuffer = renderer_.beginFrame();
            renderer_.beginSwapChainRenderPass(commandBuffer);

            auto offscreenCommandBuffer = offscreenRenderer_.beginFrame();
            offscreenRenderer_.beginSwapChainRenderPass(offscreenCommandBuffer);
            GlobalUbo ubo{};

            ubo.view = scene.camera.get_view_transformation();
            ubo.proj = scene.camera.get_proj_transformation();
            ubo.model = glm::mat4(1.0f);
            ubo.pointLight = scene.pointLight;
            ubo.cameraPos = scene.camera.position;

            if (uiManager.shadingMode == 1) {
                ubo.pointLight.position = glm::vec4(ubo.cameraPos, 1.0f);
            }

            for (auto &object_item : scene.objects) {
                for (auto model : object_item->models) {
                    ubo.model = object_item->getModelTransformation();
                    model->uniformBuffers[frameIndex]->writeToBuffer(&ubo);
                    model->uniformBuffers[frameIndex]->flush();

                    FrameInfo<VklModel> modelFrameInfo{frameIndex,
                                                       currentFrame,
                                                       offscreenCommandBuffer,
                                                       scene.camera,
                                                       &model->descriptorSets[frameIndex],
                                                       *model};

                    if (uiManager.renderMode == 0) {
                        if (uiManager.shadingMode == 0 or uiManager.shadingMode == 1) {
                            rawRenderSystem.renderObject(modelFrameInfo);
                        } else if (uiManager.shadingMode == 2) {
                            colorRenderSystem.renderObject(modelFrameInfo);
                        }
                    } else if (uiManager.renderMode == 1) {
                        wireFrameRenderSystem.renderObject(modelFrameInfo);
                    } else if (uiManager.renderMode == 2) {
                        if (model->textures_.empty())
                            rawRenderSystem.renderObject(modelFrameInfo);
                        else
                            renderSystem.renderObject(modelFrameInfo);
                    }

                    normalRenderSystem.renderObject(modelFrameInfo);
                }
            }

            if (uiManager.picking_result.has_value()) {
                auto &object_picked = scene.objects[uiManager.picking_result->object_index];
                auto &model_picked = object_picked->models[uiManager.picking_result->model_index];
                auto box = model_picked->box;
                box.apply_transform(object_picked->getModelTransformation());
                auto box_trans = box.get_box_transformation();

                ubo.model = box_trans;
                boxModel.uniformBuffers[frameIndex]->writeToBuffer(&ubo);
                boxModel.uniformBuffers[frameIndex]->flush();
                FrameInfo<VklBoxModel3D> boxFrameInfo{frameIndex,
                                                      currentFrame,
                                                      offscreenCommandBuffer,
                                                      scene.camera,
                                                      &boxModel.descriptorSets[frameIndex],
                                                      boxModel};
                lineRenderSystem.renderObject(boxFrameInfo);
            }

            offscreenRenderer_.endSwapChainRenderPass(offscreenCommandBuffer);
            offscreenRenderer_.endFrame();

            /* ImGui Rendering */

            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
            renderer_.endSwapChainRenderPass(commandBuffer);

            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }

            std::vector<VkCommandBuffer> commandBuffers{commandBuffer, offscreenCommandBuffer};

            auto result = renderer_.swapChain_->submitCommandBuffers(commandBuffers, &renderer_.currentImageIndex);

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
