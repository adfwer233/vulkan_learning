#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

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