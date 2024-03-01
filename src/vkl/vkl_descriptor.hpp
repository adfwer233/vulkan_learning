#pragma once

#include <memory>
#include <unordered_map>

#include "vkl_device.hpp"

/**
 * @page devicePage Vulkan Resource Binding
 *
 * ## DescriptorSetLayout
 *
 * ## DescriptorSet
 *
 * ## DescriptorPool
 */

class VklDescriptorSetLayout {
  public:
    class Builder {
      private:
        VklDevice &device_;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

      public:
        explicit Builder(VklDevice &device) : device_(device){};

        Builder &addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags,
                            uint32_t count = 1);

        [[nodiscard]] std::unique_ptr<VklDescriptorSetLayout> build() const;
    };

  private:
    VklDevice &device_;
    VkDescriptorSetLayout descriptorSetLayout_;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;

  public:
    VklDescriptorSetLayout(VklDevice &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~VklDescriptorSetLayout();
    VklDescriptorSetLayout(const VklDescriptorSetLayout &) = delete;
    VklDescriptorSetLayout &operator=(const VklDescriptorSetLayout &) = delete;
    [[nodiscard]] VkDescriptorSetLayout getDescriptorSetLayout() const {
        return descriptorSetLayout_;
    };

    friend class VklDescriptorWriter;
};

class VklDescriptorPool {
  public:
    class Builder {
      private:
        VklDevice &device_;
        std::vector<VkDescriptorPoolSize> poolSizes{};
        uint32_t maxSets = 1000;
        VkDescriptorPoolCreateFlags poolFlags = 0;

      public:
        explicit Builder(VklDevice &device) : device_(device) {
        }

        Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder &setMaxSets(uint32_t count);
        [[nodiscard]] std::unique_ptr<VklDescriptorPool> build() const;
    };

  private:
    VklDevice &device_;
    VkDescriptorPool descriptorPool;

  public:
    VklDescriptorPool(VklDevice &device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags,
                      const std::vector<VkDescriptorPoolSize> &poolSizes);
    ~VklDescriptorPool();
    VklDescriptorPool(const VklDescriptorPool &) = delete;
    VklDescriptorPool &operator=(const VklDescriptorPool &) = delete;

    bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

    void resetPool();

    friend class VklDescriptorWriter;
};

class VklDescriptorWriter {
  private:
    VklDescriptorSetLayout &setLayout_;
    VklDescriptorPool &pool_;
    std::vector<VkWriteDescriptorSet> writes_;

  public:
    VklDescriptorWriter(VklDescriptorSetLayout &setLayout, VklDescriptorPool &pool);

    VklDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    VklDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

    bool build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet &set);
};