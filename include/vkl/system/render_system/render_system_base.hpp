#pragma once

class BaseRenderSystem {
public:
    std::unique_ptr<VklDescriptorSetLayout> descriptorSetLayout;
    std::unique_ptr<VklDescriptorPool> descriptorPool;
};