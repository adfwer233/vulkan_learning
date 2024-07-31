#pragma once

#include <memory>
#include <vector>

#include "../core/vkl_buffer.hpp"

class VklBufferDestroyList {
    std::vector<std::unique_ptr<VklBuffer>> buffer_to_destroy;

  public:
    void destroyBuffer(std::unique_ptr<VklBuffer> &buffer) {
        buffer_to_destroy.push_back(std::move(buffer));
    }

    static VklBufferDestroyList *instance() {
        if (instance_ == nullptr)
            instance_ = new VklBufferDestroyList();
        return instance_;
    }

  private:
    VklBufferDestroyList() = default;
    static inline VklBufferDestroyList *instance_ = nullptr;
};