#pragma once
#include <vector>
#include "../core/vkl_renderer.hpp"
#include "meta_programming/type_list.hpp"


struct RenderGraphRenderPass{};
struct RenderGraphComputePass{};
struct RenderGraphTransferPass{};
struct RenderGraphImagePresentPass{};
struct RenderGraphFrameSyncBeginPass{};
struct RenderGraphFrameSyncEndPass{};

using RenderGraphPassTypeList = TypeList <
        RenderGraphRenderPass,
        RenderGraphComputePass,
        RenderGraphImagePresentPass,
        RenderGraphTransferPass,
        RenderGraphFrameSyncBeginPass,
        RenderGraphFrameSyncEndPass
    >;

template<typename T>
concept RenderGraphPass = TypeListFunctions::IsAnyOf<RenderGraphPassTypeList, T>::value;

template<RenderGraphPass T>
struct RenderGraphPassDescriptor {};

template<> struct RenderGraphPassDescriptor<RenderGraphRenderPass> {
    struct Attachment {
        VkImageView imageView;
        VkAttachmentLoadOp loadOp;
        VkClearValue clearValue;
    };

private:
    std::vector<Attachment> colorAttachments;
    Attachment depthAttachment;

    std::vector<VkImageView> inputImageViews;
    std::vector<VkImageView> inoutStorageImageViews;
    std::vector<VkBuffer> vertexBuffers;
    std::vector<VkBuffer> indexBuffers;
    std::vector<VkBuffer> inoutStorageBuffers;

    VkExtent2D renderAreaExtent;

    std::function<void()> recordFunction;

    friend class RenderGraph;
};

template<> struct RenderGraphPassDescriptor<RenderGraphImagePresentPass> {
    RenderGraphPassDescriptor<RenderGraphImagePresentPass> &setImage(VkImageView presentImageView) {
        presentImageView_ = presentImageView;
        return *this;
    }
private:
    VkImageView presentImageView_;
    friend class RenderGraph;
};

struct RenderGraph {

    using RenderGraphPassDescriptorTypeList = RenderGraphPassTypeList::monad<RenderGraphPassDescriptor>;
    using RenderGraphPassDescriptorVectors = RenderGraphPassDescriptorTypeList::monad<std::vector>::to<std::tuple>;

    /*
     * Pass Descriptors
     */

    RenderGraphPassDescriptorVectors passDescriptors;

    /*
     * Tasks
     */
    struct TaskBase {
        size_t index;
        virtual size_t TaskTypeIndex() {
            return 0;
        }
    };

    template<RenderGraphPass T>
    consteval static size_t PassTypeIndex() {
        return TypeListFunctions::IndexOf<RenderGraphPassDescriptorTypeList, RenderGraphPassDescriptor<T>>::value;
    }

    template<RenderGraphPass T>
    struct Task: TaskBase {
        size_t TaskTypeIndex() override {
            return PassTypeIndex<T>();
        }
    };

    std::vector<TaskBase> tasks;

    template<RenderGraphPass T>
    void addPass(RenderGraphPassDescriptor<T> &&descriptor){
        Task<T> task;
        auto& descriptorVector = std::get<PassTypeIndex<RenderGraphRenderPass>()>(passDescriptors);
        task.index = descriptor.size();
        tasks.push_back(task);

        descriptorVector.push_back(descriptor);
    }

    void execute(VkCommandBuffer commandBuffer) {
        for (auto task: tasks) {
            if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphRenderPass>()) {
                auto &descriptor = std::get<PassTypeIndex<RenderGraphRenderPass>()>(passDescriptors)[task.index];

                descriptor.recordFunction();
            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphComputePass>()) {
                auto &descriptor = std::get<PassTypeIndex<RenderGraphComputePass>()>(passDescriptors)[task.index];
            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphTransferPass>()) {
                auto &descriptor = std::get<PassTypeIndex<RenderGraphTransferPass>()>(passDescriptors)[task.index];
            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphImagePresentPass>()) {
                auto &descriptor = std::get<PassTypeIndex<RenderGraphImagePresentPass>()>(passDescriptors)[task.index];
            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphFrameSyncBeginPass>()) {
                auto &descriptor = std::get<PassTypeIndex<RenderGraphFrameSyncBeginPass>()>(passDescriptors)[task.index];
            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphFrameSyncEndPass>()) {
                auto &descriptor = std::get<PassTypeIndex<RenderGraphFrameSyncEndPass>()>(passDescriptors)[task.index];
            }
        }
    }
};