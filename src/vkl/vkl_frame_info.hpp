#pragma once

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"

struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.f};
    glm::mat4 normalMatrix{1.f};
};

struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
//    LveCamera &camera;
    VkDescriptorSet globalDescriptorSet;
//    LveGameObject::Map &gameObjects;
};