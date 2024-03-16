#include "application.hpp"
#include "vkl/vkl_descriptor.hpp"
#include "vkl/vkl_object.hpp"
#include "vkl/vkl_scene.hpp"
#include "vkl/vkl_box.hpp"
#include "vkl/vkl_box_model.hpp"

#include "demo/utils/controller.hpp"
#include "vkl/system/simple_render_system.hpp"
#include "vkl/system/simple_wireframe_render_system.hpp"
#include "vkl/system/line_render_system.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <functional>

static ImGui_ImplVulkanH_Window g_MainWindowData;

Application::~Application() {
}

void Application::run() {

    /** tmp model data */
    const std::vector<Vertex3D> vertexData = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.1f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.1f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.1f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.1f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.1f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.1f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.1f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.1f}, {1.0f, 1.0f}}};

    const std::vector<VklModel::index_type> indices = {{0, 1, 2}, {2, 3, 0}, {4, 5, 6}, {6, 7, 4}};

    VklModel::BuilderFromImmediateData builder;
    builder.vertices = vertexData;
    builder.indices = indices;
    builder.texturePaths = {std::format("{}/blending_transparent_window.png", DATA_DIR)};

    VklModel model(device_, builder);

    VklObject::ImportBuilder objectBuilder(std::format("{}/nanosuit/nanosuit.obj", DATA_DIR));

    auto texture = model.textures_[0];
    auto imageInfo = texture->descriptorInfo();
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

    std::vector<VkDescriptorSet> globalDescriptorSets(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uniformBuffers[i]->descriptorInfo();
        VklDescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .writeImage(1, &imageInfo)
            .build(globalDescriptorSets[i]);
    }

    /** set camera */

    VklScene scene(device_, {0, 0, 3}, {0, 1, 0});
    scene.addObject(objectBuilder);

    auto boxModel = VklBoxModel3D(device_, getStandardBox3D());
    boxModel.allocDescriptorSets(*globalSetLayout, *globalPool);

    for (auto& object: scene.objects) {
        object->allocDescriptorSets(*globalSetLayout, *globalPool);
    }

    KeyboardCameraController::setCamera(scene.camera);

    GLFWwindow *window = window_.getGLFWwindow();

    glfwSetCursorPosCallback(window, KeyboardCameraController::mouse_callback);
    glfwSetScrollCallback(window, KeyboardCameraController::scroll_callback);
    glfwSetMouseButtonCallback(window, KeyboardCameraController::mouse_button_callback);

    /** render system */
    SimpleRenderSystem<VklModel::vertex_type> renderSystem(device_, renderer_.getSwapChainRenderPass(),
                                                           globalSetLayout->getDescriptorSetLayout());

    SimpleRenderSystem<VklModel::vertex_type> rawRenderSystem(device_, renderer_.getSwapChainRenderPass(),
                                                           globalSetLayout->getDescriptorSetLayout(), std::format("{}/simple_shader.vert.spv", SHADER_DIR), std::format("{}/point_light_shader.frag.spv", SHADER_DIR));

    SimpleWireFrameRenderSystem<VklModel::vertex_type> wireFrameRenderSystem(device_, renderer_.getSwapChainRenderPass(),
                                                                             globalSetLayout->getDescriptorSetLayout(), std::format("{}/simple_shader.vert.spv", SHADER_DIR), std::format("{}/point_light_shader.frag.spv", SHADER_DIR));

    LineRenderSystem<VklBoxModel3D::vertex_type> lineRenderSystem(device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), std::format("{}/line_shader.vert.spv", SHADER_DIR), std::format("{}/line_shader.frag.spv", SHADER_DIR));

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

    bool show_demo_window = true;

    KeyboardCameraController::set_scene(scene);
    int triangle_num = 0;
    for (const auto &object_item : scene.objects) {
        triangle_num += object_item->get_triangle_num();
    }

    int render_mode = 0;

    while (not window_.shouldClose()) {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        KeyboardCameraController::processInput(window_.getGLFWwindow(), deltaTime);

        if (auto commandBuffer = renderer_.beginFrame()) {
            int frameIndex = renderer_.getFrameIndex();

            ImGui::Begin("Messages");
            ImGui::SeparatorText("Scene Information");
            ImGui::LabelText("# Triangles", "%d", triangle_num);
            ImGui::LabelText(
                "Camera Position",
                std::format("{:.3f}, {:.3f}, {:.3f}", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z).c_str());
            ImGui::LabelText("Point Light Position", std::format("{:.3f}, {:.3f}, {:.3f}", scene.pointLight.position.x, scene.pointLight.position.y, scene.pointLight.position.z).c_str());
            ImGui::SeparatorText("Performance");
            ImGui::LabelText("FPS", std::format("{:.3f}", 1 / deltaTime).c_str());
            ImGui::End();

            ImGui::Begin("Rendering Options");
            ImGui::RadioButton("Raw", &render_mode, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Wire Frame", &render_mode, 1);
            ImGui::SameLine();
            ImGui::RadioButton("With Texture", &render_mode, 2);
            ImGui::End();

            ImGui::Begin("Picking Result");
            if (KeyboardCameraController::picking_result.has_value()) {
                auto &object_picked = scene.objects[KeyboardCameraController::picking_result->object_index];
                auto model_picked = object_picked->models[KeyboardCameraController::picking_result->model_index];
                ImGui::SeparatorText("Picking Information");
                ImGui::LabelText("Object Index", "%zu", KeyboardCameraController::picking_result->object_index);
                ImGui::LabelText("Model Index", "%zu", KeyboardCameraController::picking_result->model_index);
                ImGui::LabelText("Face Index", "%zu", KeyboardCameraController::picking_result->face_index);
                ImGui::LabelText("U", "%.3f", KeyboardCameraController::picking_result->u);
                ImGui::LabelText("V", "%.3f", KeyboardCameraController::picking_result->v);
                ImGui::LabelText("W", "%.3f", KeyboardCameraController::picking_result->w);

                ImGui::SeparatorText("Object Translation");
                ImGui::SliderFloat("x", &object_picked->modelTranslation.x, -5.0f, 5.0);
                ImGui::SliderFloat("y", &object_picked->modelTranslation.y, -5.0f, 5.0);
                ImGui::SliderFloat("z", &object_picked->modelTranslation.z, -5.0f, 5.0);

                ImGui::SeparatorText("Object Scaling");
                ImGui::SliderFloat("S x", &object_picked->modelScaling.x, -0.0f, 3.0);
                ImGui::SliderFloat("S y", &object_picked->modelScaling.y, -0.0f, 3.0);
                ImGui::SliderFloat("S z", &object_picked->modelScaling.z, -0.0f, 3.0);

                ImGui::SeparatorText("Object Scaling");
                ImGui::SliderFloat("R w", &object_picked->modelRotation.w, -0.0f, 1.0);
                ImGui::SliderFloat("R x", &object_picked->modelRotation.x, -0.0f, 1.0);
                ImGui::SliderFloat("R y", &object_picked->modelRotation.y, -0.0f, 1.0);
                ImGui::SliderFloat("R z", &object_picked->modelRotation.z, -0.0f, 1.0);
            }
            ImGui::End();

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

                    FrameInfo<VklModel> modelFrameInfo{
                        frameIndex, currentFrame, commandBuffer, scene.camera, &model->descriptorSets[frameIndex], *model};

                    if (render_mode == 0) {
                        rawRenderSystem.renderObject(modelFrameInfo);
                    } else if (render_mode == 1) {
                        wireFrameRenderSystem.renderObject(modelFrameInfo);
                    } else if (render_mode == 2) {
                        renderSystem.renderObject(modelFrameInfo);
                    }
                }
            }

            if (KeyboardCameraController::picking_result.has_value()) {
                auto &object_picked = scene.objects[KeyboardCameraController::picking_result->object_index];
                auto &model_picked = object_picked->models[KeyboardCameraController::picking_result->model_index];
                auto box = model_picked->box;
                box.apply_transform(object_picked->getModelTransformation());
                auto box_trans = box.get_box_transformation();

                ubo.model = box_trans;
                boxModel.uniformBuffers[frameIndex]->writeToBuffer(&ubo);
                boxModel.uniformBuffers[frameIndex]->flush();
                FrameInfo<VklBoxModel3D> boxFrameInfo{
                        frameIndex, currentFrame, commandBuffer, scene.camera, &boxModel.descriptorSets[frameIndex],
                        boxModel
                };
                lineRenderSystem.renderObject(boxFrameInfo);
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
