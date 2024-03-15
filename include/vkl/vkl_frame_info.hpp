#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

#include "vkl_camera.hpp"

#include "templates/vkl_concept.hpp"

struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.f};
    glm::mat4 normalMatrix{1.f};
};

struct PointLight {
    glm::vec4 position;
    glm::vec4 color;
};

struct GlobalUbo {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;

    glm::vec3 cameraPos;
    PointLight pointLight;
};

template <VklRenderable RenderableModel> struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera &camera;
    VkDescriptorSet *pGlobalDescriptorSet;
    RenderableModel &model;
};