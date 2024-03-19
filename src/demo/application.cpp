#include "application.hpp"
#include "vkl/vkl_box.hpp"
#include "vkl/vkl_box_model.hpp"
#include "vkl/vkl_descriptor.hpp"
#include "vkl/vkl_image.hpp"
#include "vkl/vkl_object.hpp"
#include "vkl/vkl_scene.hpp"

#include "demo/utils/controller.hpp"
#include "vkl/system/line_render_system.hpp"
#include "vkl/system/simple_render_system.hpp"
#include "vkl/system/simple_wireframe_render_system.hpp"

#include "vkl/system/base_compute_system.hpp"
#include "vkl/system/path_tracing_compute_system.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <functional>

#include "ui/ui_manager.hpp"

static ImGui_ImplVulkanH_Window g_MainWindowData;

Application::~Application() {
}

void Application::run() {

    /** tmp model data */
    const std::vector<Vertex3D> vertexData = {
        {{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.1f}, {1.0f, 0.0f}},
        {{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.1f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.1f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.1f}, {1.0f, 1.0f}}};

    const std::vector<VklModel::index_type> indices = {{0, 1, 2}, {2, 3, 0}};

    VklModel::BuilderFromImmediateData builder;
    builder.vertices = vertexData;
    builder.indices = indices;
    //    builder.texturePaths = {std::format("{}/blending_transparent_window.png", DATA_DIR)};

    VklModel model(device_, builder);

    VklObject::ImportBuilder objectBuilder(std::format("{}/nanosuit/nanosuit.obj", DATA_DIR));

    VklObject::ImportBuilder lightSourceBuilder(std::format("{}/light/light_source.obj", DATA_DIR));
    VklObject::ImportBuilder model1Builder(std::format("{}/models/model1.obj", DATA_DIR));

    VklScene scene(device_, {0, 0, 3}, {0, 1, 0});
    scene.addObject(lightSourceBuilder);
    scene.addObject(objectBuilder);

    scene.addObject(model1Builder);

    scene.setMaterial(1, 1);
    scene.setMaterial(0, 3);
    scene.setMaterial(2, 2);
    scene.objects[0]->modelTranslation = glm::vec3(0, -4, 0);
    scene.objects[2]->modelScaling = glm::vec3(1, 2, 3);
    scene.objects[2]->modelTranslation = glm::vec3(-1.5, -1, 0);

    PathTracingComputeModel pathTracingComputeModel(device_, scene);

    PathTracingComputeSystem pathTracingComputeSystem(device_, pathTracingComputeModel);

    auto targetTexture = pathTracingComputeModel.getTargetTexture();
    auto accumulationTexture = pathTracingComputeModel.getAccumulationTexture();

    model.addTextureFromImage(targetTexture);

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

    model.allocDescriptorSets(*globalSetLayout, *globalPool);

    auto boxModel = VklBoxModel3D(device_, getStandardBox3D());
    boxModel.allocDescriptorSets(*globalSetLayout, *globalPool);

    for (auto &object : scene.objects) {
        object->allocDescriptorSets(*globalSetLayout, *globalPool);
    }

    KeyboardCameraController::setCamera(scene.camera);

    KeyboardCameraController::actionCallBack = [&]() {
        std::cout << "reset sampling" << std::endl;
        pathTracingComputeSystem.computeModel_.ubo.currentSample = 0;
    };

    GLFWwindow *window = window_.getGLFWwindow();

    glfwSetCursorPosCallback(window, KeyboardCameraController::mouse_callback);
    glfwSetScrollCallback(window, KeyboardCameraController::scroll_callback);
    glfwSetMouseButtonCallback(window, KeyboardCameraController::mouse_button_callback);

    /** render system */
    SimpleRenderSystem<VklModel::vertex_type> renderSystem(device_, renderer_.getSwapChainRenderPass(),
                                                           globalSetLayout->getDescriptorSetLayout());

    SimpleRenderSystem<VklModel::vertex_type> rawRenderSystem(
        device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/simple_shader.vert.spv", SHADER_DIR),
        std::format("{}/point_light_shader.frag.spv", SHADER_DIR));

    SimpleWireFrameRenderSystem<VklModel::vertex_type> wireFrameRenderSystem(
        device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/simple_shader.vert.spv", SHADER_DIR),
        std::format("{}/point_light_shader.frag.spv", SHADER_DIR));

    LineRenderSystem<VklBoxModel3D::vertex_type> lineRenderSystem(
        device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/line_shader.vert.spv", SHADER_DIR), std::format("{}/line_shader.frag.spv", SHADER_DIR));

    SimpleRenderSystem<VklModel::vertex_type> backGroundRenderSystem(
        device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
        std::format("{}/simple_shader.vert.spv", SHADER_DIR),
        std::format("{}/path_tracing_post_shader.frag.spv", SHADER_DIR));

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

    UIManager uiManager(scene, pathTracingComputeModel);

    KeyboardCameraController::set_scene(scene);
    KeyboardCameraController::setUIManager(uiManager);

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

        uiManager.renderImgui();

        if (uiManager.renderMode == 3) {

            auto commandBuffer = renderer_.beginFrame();

            VkImageMemoryBarrier read2Gen = VklImageUtils::ReadOnlyToGeneralBarrier(targetTexture);

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &read2Gen);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                              pathTracingComputeSystem.pipeline_->computePipeline_);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                                    pathTracingComputeSystem.pipelineLayout_, 0, 1,
                                    &pathTracingComputeSystem.computeDescriptorSets[frameIndex], 0, nullptr);

            pathTracingComputeSystem.computeModel_.ubo.cameraPosition = scene.camera.position;
            pathTracingComputeSystem.computeModel_.ubo.cameraUp = scene.camera.camera_up_axis;
            pathTracingComputeSystem.computeModel_.ubo.cameraFront = scene.camera.camera_front;
            pathTracingComputeSystem.computeModel_.ubo.currentSample += 1;

            pathTracingComputeSystem.updateUniformBuffer(frameIndex);

            auto [local_x, local_y, local_z] = pathTracingComputeSystem.computeModel_.getLocalSize();
            auto [x, y, z] = pathTracingComputeSystem.computeModel_.getSize();

            vkCmdDispatch(commandBuffer, x / local_x, y / local_y, z / local_z);

            VkImageMemoryBarrier gen2TranSrc = VklImageUtils::generalToTransferSrcBarrier(targetTexture);

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &gen2TranSrc);

            VkImageMemoryBarrier gen2TranDst = VklImageUtils::generalToTransferDstBarrier(accumulationTexture);

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &gen2TranDst);

            VkImageCopy region = VklImageUtils::imageCopyRegion(1024, 1024);
            vkCmdCopyImage(commandBuffer, targetTexture, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, accumulationTexture,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

            VkImageMemoryBarrier tranDst2Gen = VklImageUtils::transferDstToGeneralBarrier(accumulationTexture);

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &tranDst2Gen);

            VkImageMemoryBarrier tranSrc2ReadOnly = VklImageUtils::transferSrcToReadOnlyBarrier(targetTexture);

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                                 &tranSrc2ReadOnly);

            renderer_.beginSwapChainRenderPass(commandBuffer);

            GlobalUbo ubo{};
            ubo.view =
                glm::lookAt(glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 0.5f}, glm::vec3{0.0f, -1.0f, 0.0f});
            ubo.proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
            ubo.model = glm::mat4(1.0f);
            ubo.pointLight = scene.pointLight;
            ubo.cameraPos = scene.camera.position;
            model.uniformBuffers[frameIndex]->writeToBuffer(&ubo);
            model.uniformBuffers[frameIndex]->flush();

            FrameInfo<VklModel> modelFrameInfo{
                frameIndex, currentFrame, commandBuffer, scene.camera, &model.descriptorSets[frameIndex], model};

            backGroundRenderSystem.renderObject(modelFrameInfo);

            /* ImGui Rendering */
            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

            renderer_.endSwapChainRenderPass(commandBuffer);
            renderer_.endFrame();

        } else {

            auto commandBuffer = renderer_.beginFrame();

            if (uiManager.renderMode != 3) {
                renderer_.beginSwapChainRenderPass(commandBuffer);
                GlobalUbo ubo{};

                ubo.view = scene.camera.get_view_transformation();
                ubo.proj = scene.camera.get_proj_transformation();
                ubo.model = glm::mat4(1.0f);
                ubo.pointLight = scene.pointLight;
                ubo.cameraPos = scene.camera.position;

                for (auto &object_item : scene.objects) {
                    for (auto model : object_item->models) {
                        ubo.model = object_item->getModelTransformation();
                        model->uniformBuffers[frameIndex]->writeToBuffer(&ubo);
                        model->uniformBuffers[frameIndex]->flush();

                        FrameInfo<VklModel> modelFrameInfo{frameIndex,
                                                           currentFrame,
                                                           commandBuffer,
                                                           scene.camera,
                                                           &model->descriptorSets[frameIndex],
                                                           *model};

                        if (uiManager.renderMode == 0) {
                            rawRenderSystem.renderObject(modelFrameInfo);
                        } else if (uiManager.renderMode == 1) {
                            wireFrameRenderSystem.renderObject(modelFrameInfo);
                        } else if (uiManager.renderMode == 2) {
                            if (model->textures_.empty())
                                rawRenderSystem.renderObject(modelFrameInfo);
                            else
                                renderSystem.renderObject(modelFrameInfo);
                        }
                    }
                }

                if (uiManager.picking_result.has_value()) {
                    auto &object_picked = scene.objects[uiManager.picking_result->object_index];
                    auto &model_picked =
                        object_picked->models[uiManager.picking_result->model_index];
                    auto box = model_picked->box;
                    box.apply_transform(object_picked->getModelTransformation());
                    auto box_trans = box.get_box_transformation();

                    ubo.model = box_trans;
                    boxModel.uniformBuffers[frameIndex]->writeToBuffer(&ubo);
                    boxModel.uniformBuffers[frameIndex]->flush();
                    FrameInfo<VklBoxModel3D> boxFrameInfo{
                        frameIndex, currentFrame, commandBuffer, scene.camera, &boxModel.descriptorSets[frameIndex],
                        boxModel};
                    lineRenderSystem.renderObject(boxFrameInfo);
                }
            }

            /* ImGui Rendering */
            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

            renderer_.endSwapChainRenderPass(commandBuffer);
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
