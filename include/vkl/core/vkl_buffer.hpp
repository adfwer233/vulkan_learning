#pragma once

#include "vkl/core/vkl_device.hpp"

class VklBuffer {

  private:
    static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    VklDevice &device_;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    void *mapped = nullptr;

    VkDeviceSize bufferSize;
    uint32_t instanceCount;
    VkDeviceSize instanceSize;
    VkDeviceSize alignmentSize;
    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryPropertyFlags;

  public:
    VklBuffer(VklDevice &device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags,
              VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);

    ~VklBuffer();

    VklBuffer(const VklBuffer &) = delete;
    VklBuffer &operator=(const VklBuffer &) = delete;

    [[nodiscard]] VkBuffer getBuffer() const {
        return buffer;
    }

    /**
     * @brief Map a memory range (VkDeviceMemory) to the buffer (void*)
     * @param size
     * @param offset
     * @return
     */
    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    /**
     * @brief unmap the buffer
     */
    void unmap();

    /**
     * @brief write data to the mapped buffer
     * @param data
     * @param size
     * @param offset
     */
    void writeToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    /**
     * @brief flush buffer
     * @param size
     * @param offset
     * @return
     */
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    /**
     * @brief
     * @param size
     * @param offset
     * @return
     */
    VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
};