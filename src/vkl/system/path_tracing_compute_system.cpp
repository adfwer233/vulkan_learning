// #include "vkl/system/path_tracing_compute_system.hpp"
//
// #include "vkl/vkl_swap_chain.hpp"
//
// PathTracingComputeSystem::PathTracingComputeSystem(VklDevice &device): device_(device) {
//     createComputeDescriptorSetLayout();
//     createPipelineLayout();
//     createPipeline();
//     createUniformBuffers();
//     createDescriptorPool();
//     createComputeDescriptorSets();
//     createCommandBuffer();
//     createSyncObjects();
// }
//
// void PathTracingComputeSystem::createComputeDescriptorSetLayout() {
//     std::vector<VkDescriptorSetLayoutBinding> layoutBindings(7);
//
//     layoutBindings[0].binding = 0;
//     layoutBindings[0].descriptorCount = 1;
//     layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//     layoutBindings[0].pImmutableSamplers = nullptr;
//     layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//
//     layoutBindings[1].binding = 1;
//     layoutBindings[1].descriptorCount = 1;
//     layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//     layoutBindings[1].pImmutableSamplers = nullptr;
//     layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//
//     layoutBindings[2].binding = 2;
//     layoutBindings[2].descriptorCount = 1;
//     layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//     layoutBindings[2].pImmutableSamplers = nullptr;
//     layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//
//     layoutBindings[3].binding = 3;
//     layoutBindings[3].descriptorCount = 1;
//     layoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//     layoutBindings[3].pImmutableSamplers = nullptr;
//     layoutBindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//
//     layoutBindings[4].binding = 4;
//     layoutBindings[4].descriptorCount = 1;
//     layoutBindings[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//     layoutBindings[4].pImmutableSamplers = nullptr;
//     layoutBindings[4].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//
//     layoutBindings[5].binding = 5;
//     layoutBindings[5].descriptorCount = 1;
//     layoutBindings[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//     layoutBindings[5].pImmutableSamplers = nullptr;
//     layoutBindings[5].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//
//     layoutBindings[6].binding = 6;
//     layoutBindings[6].descriptorCount = 1;
//     layoutBindings[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//     layoutBindings[6].pImmutableSamplers = nullptr;
//     layoutBindings[6].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//
//     VkDescriptorSetLayoutCreateInfo layoutCreateInfo;
//     layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//     layoutCreateInfo.bindingCount = 7;
//     layoutCreateInfo.pBindings = layoutBindings.data();
//
//     if (vkCreateDescriptorSetLayout(device_.device(), &layoutCreateInfo, nullptr, &computeDescriptorSetLayout) !=
//     VK_SUCCESS) {
//         throw std::runtime_error("failed to create compute descriptor set layout");
//     }
// }
//
// void PathTracingComputeSystem::createPipelineLayout() {
//     VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//     pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//     pipelineLayoutInfo.setLayoutCount = 1;
//     pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout;
//
//     if (vkCreatePipelineLayout(device_.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
//         throw std::runtime_error("failed to create compute pipeline layout!");
//     }
// }
//
// void PathTracingComputeSystem::createPipeline() {
//     ComputePipelineConfigInfo configInfo{computeDescriptorSetLayout, pipelineLayout_};
//     this->pipeline_ = std::make_unique<VklComputePipeline>(device_, comp_shader_path, configInfo);
// }
//
// void PathTracingComputeSystem::createCommandBuffer() {
//     computeCommandBuffers.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
//
//     VkCommandBufferAllocateInfo allocateInfo{};
//     allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//     allocateInfo.commandPool = device_.getCommandPool();
//     allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//     allocateInfo.commandBufferCount = (uint32_t)computeCommandBuffers.size();
//
//     if (vkAllocateCommandBuffers(device_.device(), &allocateInfo, computeCommandBuffers.data()) != VK_SUCCESS) {
//         throw std::runtime_error("failed to allocate compute command buffers!");
//     }
// }
//
// void PathTracingComputeSystem::createSyncObjects() {
//     computeFinishedSemaphores.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
//     computeInFlightFences.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
//
//     VkSemaphoreCreateInfo semaphoreInfo{};
//     semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//
//     VkFenceCreateInfo fenceInfo{};
//     fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//     fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//     for (size_t i = 0; i < VklSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
//         if (vkCreateSemaphore(device_.device(), &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS
//         ||
//             vkCreateFence(device_.device(), &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
//             throw std::runtime_error("failed to create compute synchronization objects for a frame!");
//         }
//     }
// }
//
// void PathTracingComputeSystem::createUniformBuffers() {
//     VkDeviceSize bufferSize = sizeof(PathTracingComputeSystem::UniformBufferObject);
//
//     for (size_t i = 0; i < VklSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
//         auto buffer =
//                 std::make_unique<VklBuffer>(device_, bufferSize, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
//         buffer->map();
//         uniformBuffers_.push_back(std::move(buffer));
//     }
// }
//
// void PathTracingComputeSystem::createDescriptorPool() {
//     std::array<VkDescriptorPoolSize, 3> poolSizes{};
//     poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//     poolSizes[0].descriptorCount = static_cast<uint32_t>(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
//
//     poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//     poolSizes[1].descriptorCount = static_cast<uint32_t>(VklSwapChain::MAX_FRAMES_IN_FLIGHT) * 2;
//
//     poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//     poolSizes[2].descriptorCount = static_cast<uint32_t>(VklSwapChain::MAX_FRAMES_IN_FLIGHT) * 4;
//
//     VkDescriptorPoolCreateInfo poolInfo{};
//     poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//     poolInfo.poolSizeCount = 3;
//     poolInfo.pPoolSizes = poolSizes.data();
//     poolInfo.maxSets = static_cast<uint32_t>(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
//
//     if (vkCreateDescriptorPool(device_.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
//         throw std::runtime_error("failed to create descriptor pool!");
//     }
// }
//
// void PathTracingComputeSystem::createComputeDescriptorSets() {
//     std::vector<VkDescriptorSetLayout> layouts(VklSwapChain::MAX_FRAMES_IN_FLIGHT, computeDescriptorSetLayout);
//     VkDescriptorSetAllocateInfo allocInfo{};
//     allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//     allocInfo.descriptorPool = descriptorPool;
//     allocInfo.descriptorSetCount = static_cast<uint32_t>(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
//     allocInfo.pSetLayouts = layouts.data();
//
//     computeDescriptorSets.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
//     if (vkAllocateDescriptorSets(device_.device(), &allocInfo, computeDescriptorSets.data()) != VK_SUCCESS) {
//         throw std::runtime_error("failed to allocate descriptor sets!");
//     }
//
//     for (size_t i = 0; i < VklSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
//         VkDescriptorBufferInfo uniformBufferInfo{};
//         uniformBufferInfo.buffer = uniformBuffers_[i]->getBuffer();
//         uniformBufferInfo.offset = 0;
//         uniformBufferInfo.range = sizeof(UniformBufferObject);
//
//         std::array<VkWriteDescriptorSet, 7> descriptorWrites{};
//         descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptorWrites[0].dstSet = computeDescriptorSets[i];
//         descriptorWrites[0].dstBinding = 0;
//         descriptorWrites[0].dstArrayElement = 0;
//         descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//         descriptorWrites[0].descriptorCount = 1;
//         descriptorWrites[0].pBufferInfo = &uniformBufferInfo;
//
//         VkDescriptorImageInfo targetTextureInfo{};
//         // todo: get image info
//
//         VkDescriptorBufferInfo storageBufferInfoLastFrame{};
//         storageBufferInfoLastFrame.buffer = model.getVertexBuffer((i - 1) % VklSwapChain::MAX_FRAMES_IN_FLIGHT);
//         storageBufferInfoLastFrame.offset = 0;
//         storageBufferInfoLastFrame.range = sizeof(Particle) * particle_number_;
//
//         descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptorWrites[1].dstSet = computeDescriptorSets[i];
//         descriptorWrites[1].dstBinding = 1;
//         descriptorWrites[1].dstArrayElement = 0;
//         descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//         descriptorWrites[1].descriptorCount = 1;
//         descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;
//
//         VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
//         storageBufferInfoCurrentFrame.buffer = model.getVertexBuffer(i);
//         storageBufferInfoCurrentFrame.offset = 0;
//         storageBufferInfoCurrentFrame.range = sizeof(Particle) * particle_number_;
//
//         descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptorWrites[2].dstSet = computeDescriptorSets[i];
//         descriptorWrites[2].dstBinding = 2;
//         descriptorWrites[2].dstArrayElement = 0;
//         descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//         descriptorWrites[2].descriptorCount = 1;
//         descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;
//
//         vkUpdateDescriptorSets(device_.device(), 3, descriptorWrites.data(), 0, nullptr);
//     }
// }