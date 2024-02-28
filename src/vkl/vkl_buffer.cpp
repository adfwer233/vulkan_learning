#include "vkl_buffer.hpp"

VkDeviceSize VklBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

VklBuffer::VklBuffer(VklDevice &device, VkDeviceSize instanceSize, uint32_t instanceCount,
                     VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
                     VkDeviceSize minOffsetAlignment)
    : device_(device), instanceSize(instanceSize), instanceCount(instanceCount), usageFlags(usageFlags),
      memoryPropertyFlags(memoryPropertyFlags) {
    this->alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    this->bufferSize = alignmentSize * instanceCount;
    device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}

VklBuffer::~VklBuffer() {
    unmap();
    vkDestroyBuffer(device_.device(), this->buffer, nullptr);
    vkFreeMemory(device_.device(), memory, nullptr);
}

VkResult VklBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
    return vkMapMemory(device_.device(), memory, offset, size, 0, &mapped);
}

void VklBuffer::unmap() {
    if (mapped) {
        vkUnmapMemory(device_.device(), memory);
        mapped = nullptr;
    }
}

void VklBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
    if (size == VK_WHOLE_SIZE) {
        memcpy(mapped, data, bufferSize);
    } else {
        char *memOffset = (char *)mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}
