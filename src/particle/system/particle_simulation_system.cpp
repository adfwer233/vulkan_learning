#include "particle/system/particle_simulation_system.hpp"
#include <array>

ParticleSimulationSystem::ParticleSimulationSystem(VklDevice &device, VklModelTemplate<Particle> &model)
    : device_(device), model(model) {
    createComputeDescriptorSetLayout();
    createPipelineLayout();
    createPipeline();
    createUniformBuffers();
    createDescriptorPool();
    createComputeDescriptorSets();
    createCommandBuffer();
    createSyncObjects();
}

void ParticleSimulationSystem::createComputeDescriptorSetLayout() {
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings(3);
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].pImmutableSamplers = nullptr;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorCount = 1;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings[1].pImmutableSamplers = nullptr;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    layoutBindings[2].binding = 2;
    layoutBindings[2].descriptorCount = 1;
    layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings[2].pImmutableSamplers = nullptr;
    layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 3;
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(device_.device(), &layoutInfo, nullptr, &computeDescriptorSetLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }
}

void ParticleSimulationSystem::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout;

    if (vkCreatePipelineLayout(device_.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline layout!");
    }
}

void ParticleSimulationSystem::createPipeline() {
    ComputePipelineConfigInfo configInfo{computeDescriptorSetLayout, pipelineLayout_};
    this->pipeline_ = std::make_unique<VklComputePipeline>(device_, comp_shader_path, configInfo);
}

void ParticleSimulationSystem::createCommandBuffer() {
    computeCommandBuffers.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = device_.getCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)computeCommandBuffers.size();

    if (vkAllocateCommandBuffers(device_.device(), &allocInfo, computeCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate compute command buffers!");
    }
}

void ParticleSimulationSystem::createSyncObjects() {
    computeFinishedSemaphores.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    computeInFlightFences.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < VklSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device_.device(), &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device_.device(), &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        }
    }
}

void ParticleSimulationSystem::recordComputeCommandBuffer(size_t frameIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(computeCommandBuffers[frameIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording compute command buffer!");
    }

    vkCmdBindPipeline(computeCommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_->computePipeline_);

    vkCmdBindDescriptorSets(computeCommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout_, 0, 1,
                            &computeDescriptorSets[frameIndex], 0, nullptr);

    vkCmdDispatch(computeCommandBuffers[frameIndex], 1024 / 256, 1, 1);

    if (vkEndCommandBuffer(computeCommandBuffers[frameIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record compute command buffer!");
    }
}

void ParticleSimulationSystem::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    for (size_t i = 0; i < VklSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        auto buffer =
            std::make_unique<VklBuffer>(device_, sizeof(UniformBufferObject), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        buffer->map();
        uniformBuffers_.push_back(std::move(buffer));
    }
}

void ParticleSimulationSystem::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(VklSwapChain::MAX_FRAMES_IN_FLIGHT);

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(VklSwapChain::MAX_FRAMES_IN_FLIGHT) * 2;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(VklSwapChain::MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device_.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void ParticleSimulationSystem::createComputeDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(VklSwapChain::MAX_FRAMES_IN_FLIGHT, computeDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    computeDescriptorSets.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device_.device(), &allocInfo, computeDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < VklSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo uniformBufferInfo{};
        uniformBufferInfo.buffer = uniformBuffers_[i]->getBuffer();
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = sizeof(UniformBufferObject);

        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = computeDescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

        VkDescriptorBufferInfo storageBufferInfoLastFrame{};
        storageBufferInfoLastFrame.buffer = model.getVertexBuffer((i - 1) % VklSwapChain::MAX_FRAMES_IN_FLIGHT);
        storageBufferInfoLastFrame.offset = 0;
        storageBufferInfoLastFrame.range = sizeof(Particle) * 1024;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = computeDescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

        VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
        storageBufferInfoCurrentFrame.buffer = model.getVertexBuffer(i);
        storageBufferInfoCurrentFrame.offset = 0;
        storageBufferInfoCurrentFrame.range = sizeof(Particle) * 1024;

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = computeDescriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;

        vkUpdateDescriptorSets(device_.device(), 3, descriptorWrites.data(), 0, nullptr);
    }
}

void ParticleSimulationSystem::computeSubmission(size_t frameIndex, float deltaTime) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    vkWaitForFences(device_.device(), 1, &computeInFlightFences[frameIndex], VK_TRUE, UINT64_MAX);

    updateUniformBuffer(frameIndex, deltaTime);

    vkResetFences(device_.device(), 1, &computeInFlightFences[frameIndex]);

    vkResetCommandBuffer(computeCommandBuffers[frameIndex], /*VkCommandBufferResetFlagBits*/ 0);

    recordComputeCommandBuffer(frameIndex);

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &computeCommandBuffers[frameIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &computeFinishedSemaphores[frameIndex];

    if (vkQueueSubmit(device_.computeQueue(), 1, &submitInfo, computeInFlightFences[frameIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit compute command buffer!");
    };
}

void ParticleSimulationSystem::updateUniformBuffer(uint32_t frameIndex, float deltaTime) {
    UniformBufferObject ubo{};
    ubo.deltaTime = deltaTime * 2.0f;

    uniformBuffers_[frameIndex]->writeToBuffer(&ubo);
}

ParticleSimulationSystem::~ParticleSimulationSystem() {
    for (auto semaphore : this->computeFinishedSemaphores) {
        vkDestroySemaphore(device_.device(), semaphore, nullptr);
    }

    for (auto fence : this->computeInFlightFences) {
        vkDestroyFence(device_.device(), fence, nullptr);
    }

    vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);
    vkDestroyDescriptorSetLayout(device_.device(), computeDescriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(device_.device(), descriptorPool, nullptr);
}
