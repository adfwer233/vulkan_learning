#pragma once

#include "base_compute_system.hpp"

#include "vkl/bvh/vkl_bvh.hpp"

#include "glm/glm.hpp"

struct PathTracingUniformBufferObject {
    glm::vec3 cameraPosition;
    glm::vec3 cameraUp;
    glm::vec3 cameraFront;
    float cameraZoom;
    float time;
    uint32_t currentSample;
    uint32_t numTriangles;
    uint32_t numLights;
};

class PathTracingComputeModel {
private:
    VklDevice &device_;
    VklScene &scene_;

    std::vector<ComputeDescriptor<VklBuffer>> uniformBufferDescriptors_;
    std::vector<ComputeDescriptor<VklBuffer>> storageBufferDescriptors_;
    std::vector<ComputeDescriptor<VklTexture>> textureDescriptors_;

    std::vector<std::unique_ptr<VklBuffer>> uniformBuffers;
    std::vector<std::unique_ptr<VklBuffer>> storageBuffers;
    std::vector<std::unique_ptr<VklTexture>> textures;

public:
    PathTracingComputeModel(VklDevice &device, VklScene &scene): device_(device), scene_(scene) {
        VklBVH bvh(scene);
        auto bvhTree = bvh.createGPUBVHTree();

        /*
         * create storage buffers
         */

        // create triangle buffer
        uint32_t trianglesNum = static_cast<uint32_t>(bvh.triangles.size());
        VklBuffer stagingBuffer1{device_, sizeof(VklBVHGPUModel::Triangle), trianglesNum, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        stagingBuffer1.map();
        stagingBuffer1.writeToBuffer((void *)bvh.triangles.data());

        auto triangleBuffer = std::make_unique<VklBuffer>(device, sizeof(VklBVHGPUModel::Triangle), trianglesNum,
                                                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        device.copyBuffer(stagingBuffer1.getBuffer(), triangleBuffer->getBuffer(), sizeof(VklBVHGPUModel::Triangle) * trianglesNum);

        // create material buffer

        uint32_t materialNum = static_cast<uint32_t>(scene.materials.size());
        VklBuffer stagingBuffer2{device_, sizeof(VklBVHGPUModel::Material), materialNum, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        stagingBuffer2.map();
        stagingBuffer2.writeToBuffer((void *)scene.materials.data());

        auto materialBuffer = std::make_unique<VklBuffer>(device, sizeof(VklBVHGPUModel::Material), materialNum,
                                                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        device.copyBuffer(stagingBuffer2.getBuffer(), materialBuffer->getBuffer(), sizeof(VklBVHGPUModel::Material) * materialNum);

        // create aabb buffer

        uint32_t aabbNum = static_cast<uint32_t>(bvhTree.size());
        VklBuffer stagingBuffer3{device, sizeof(VklBVHGPUModel::BVHNode), aabbNum, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
        stagingBuffer3.map();
        stagingBuffer3.writeToBuffer((void*)bvhTree.data());

        auto aabbBuffer = std::make_unique<VklBuffer>(device, sizeof(VklBVHGPUModel::BVHNode), trianglesNum, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        device.copyBuffer(stagingBuffer3.getBuffer(), aabbBuffer->getBuffer(), sizeof(VklBVHGPUModel::BVHNode) * aabbNum);

        // create light buffer

        uint32_t lightNum = static_cast<uint32_t>(bvh.lights.size());
        VklBuffer stagingBuffer4{device, sizeof(VklBVHGPUModel::Light), lightNum, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
        stagingBuffer4.map();
        stagingBuffer4.writeToBuffer((void*)bvhTree.data());

        auto lightBuffer = std::make_unique<VklBuffer>(device, sizeof(VklBVHGPUModel::Light), lightNum, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        device.copyBuffer(stagingBuffer4.getBuffer(), lightBuffer->getBuffer(), sizeof(VklBVHGPUModel::Light) * lightNum);

        storageBuffers.push_back(std::move(triangleBuffer));
        storageBuffers.push_back(std::move(materialBuffer));
        storageBuffers.push_back(std::move(aabbBuffer));
        storageBuffers.push_back(std::move(lightBuffer));

        for (int i = 0; i < storageBuffers.size(); i++) {
            storageBufferDescriptors_.push_back({storageBuffers[i]->descriptorInfo(), VK_SHADER_STAGE_COMPUTE_BIT});
        }
    }

    std::vector<ComputeDescriptor<VklBuffer>> getUniformBufferDescriptors() {return uniformBufferDescriptors_;}
    std::vector<ComputeDescriptor<VklTexture>> getTextureDescriptors() {return textureDescriptors_;}
    std::vector<ComputeDescriptor<VklBuffer>> getStorageDescriptor() {return storageBufferDescriptors_;}

    std::tuple<int, int, int> getLocalSize() {
        return {32, 32, 1};
    }

    std::tuple<int, int, int> getSize() {
        return {1024, 1024, 1};
    }
};

