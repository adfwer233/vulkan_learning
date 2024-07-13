#pragma once
#include <vector>
#include "../core/vkl_renderer.hpp"
#include "meta_programming/type_list.hpp"

#include "../core/vkl_texture.hpp"


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

struct RenderGraphTextureAttachment {};
struct RenderGraphStorageBufferAttachment{};

using RenderGraphAttachmentTypeList = TypeList <
        RenderGraphTextureAttachment,
        RenderGraphStorageBufferAttachment
    >;

template<typename T>
concept RenderGraphPass = TypeListFunctions::IsAnyOf<RenderGraphPassTypeList, T>::value;

template<typename T>
concept RenderGraphAttachment = TypeListFunctions::IsAnyOf<RenderGraphAttachmentTypeList, T>::value;

struct RenderGraphObjectsBase {
    std::string name;
};


struct RenderGraphAttachmentDescriptorBase: public RenderGraphObjectsBase {
    virtual ~RenderGraphAttachmentDescriptorBase() = default;
};
struct RenderGraphPassDescriptorBase: public RenderGraphObjectsBase {
    std::vector<RenderGraphAttachmentDescriptorBase*> input_descriptors;
    std::vector<RenderGraphAttachmentDescriptorBase*> output_descriptors;

    virtual ~RenderGraphPassDescriptorBase() = default;
};

template<RenderGraphPass T>
struct RenderGraphPassDescriptor: public RenderGraphPassDescriptorBase {};

template<RenderGraphPass T>
struct RenderGraphPassInstance: public RenderGraphObjectsBase {};

template<RenderGraphAttachment T>
struct RenderGraphAttachmentDescriptor: public RenderGraphAttachmentDescriptorBase {};

template<RenderGraphAttachment T>
struct RenderGraphAttachmentInstance: public RenderGraphObjectsBase {};

struct RenderGraphAttachmentBase: public RenderGraphObjectsBase {
    virtual ~RenderGraphAttachmentBase() = default;
};

struct RenderGraphPassBase: public RenderGraphObjectsBase {
    RenderGraphPassDescriptorBase* descriptor_p;
    std::vector<RenderGraphAttachmentBase*> input_attachments;
    std::vector<RenderGraphAttachmentBase*> output_attachments;
};

template<RenderGraphPass T>
struct RenderGraphPassDerived: public RenderGraphPassBase {
    std::vector<RenderGraphPassInstance<T> *> instances;
};

template<RenderGraphAttachment T> struct RenderGraphAttachmentDerived: public RenderGraphAttachmentBase {
    RenderGraphAttachmentDescriptor<T> *descriptor_p;
    std::vector<RenderGraphAttachmentInstance<T> *> instances;
};

template<> struct RenderGraphAttachmentDescriptor<RenderGraphTextureAttachment> {
    std::string name;

    uint32_t width, height;

    enum class AttachmentType {
        ColorAttachment,
        DepthAttachment
    };

    AttachmentType type;

    VkClearValue clearValue;
};

template<> struct RenderGraphAttachmentInstance<RenderGraphTextureAttachment> {
    std::unique_ptr<VklTexture> texture;
};

template<> struct RenderGraphPassInstance<RenderGraphRenderPass> {
    VkFramebuffer framebuffer;
    VkRenderPass renderPass;

    std::vector<RenderGraphAttachmentInstance<RenderGraphTextureAttachment>> in;
    std::vector<RenderGraphAttachmentInstance<RenderGraphTextureAttachment>> out;
};

template<> struct RenderGraphPassDescriptor<RenderGraphRenderPass>: public RenderGraphPassDescriptorBase {
    uint32_t width, height;

    std::vector<RenderGraphAttachmentDescriptor<RenderGraphTextureAttachment>> inTextureAttachmentDescriptors;
    std::vector<RenderGraphAttachmentDescriptor<RenderGraphTextureAttachment>> outTextureAttachmentDescriptors;

    friend class RenderGraph;
};

template<> struct RenderGraphPassDescriptor<RenderGraphImagePresentPass>: public RenderGraphPassDescriptorBase {
    RenderGraphPassDescriptor<RenderGraphImagePresentPass> &setImage(VkImageView presentImageView) {
        presentImageView_ = presentImageView;
        return *this;
    }
private:
    VkImageView presentImageView_;
    friend class RenderGraph;
};

struct RenderGraphDescriptor {
    std::vector<RenderGraphPassDescriptorBase*> passDescriptors;
    std::vector<RenderGraphAttachmentDescriptorBase*> attachmentDescriptors;

    template<RenderGraphPass PassType>
    RenderGraphPassDescriptor<PassType>* pass(const std::string& name) {
        auto node = new RenderGraphPassDescriptor<PassType*>();
        node->name = name;
        passDescriptors.push_back(node);
        return node;
    }

    template<RenderGraphAttachment AttachmentType>
    AttachmentType attachment(const std::string& name) {
        auto edge = new RenderGraphAttachmentDescriptor<AttachmentType>();
        edge->name = name;
        attachmentDescriptors.push_back(edge);
        return edge;
    }
};

struct RenderGraph {

    VklDevice &device_;

    uint32_t instance_n_;

    std::vector<RenderGraphPassBase*> passes;
    std::vector<RenderGraphAttachmentBase*> attachments;

    RenderGraphDescriptor* renderGraphDescriptor_;

    static RenderGraphPassBase* pass_desc_to_obj(RenderGraphPassDescriptorBase* descriptor_p) {
        RenderGraphPassBase* res = nullptr;

        auto for_loop_body = [&]<RenderGraphPass PassType>(PassType passType) {
            if (dynamic_cast<RenderGraphPassDescriptor<PassType>*>(descriptor_p) != nullptr) {
                auto obj = new RenderGraphPassDerived<PassType>();
                obj->descriptor_p = dynamic_cast<RenderGraphPassDescriptor<PassType>*>(descriptor_p);
                res = obj;
            }
        };

        auto for_loop = [&]<RenderGraphPass... PassTypes>(TypeList<PassTypes...>) {
            (for_loop_body(PassTypes()), ...);
        };

        for_loop(RenderGraphPassTypeList());

        return res;
    }

    static RenderGraphAttachmentBase* attachment_desc_to_obj(RenderGraphAttachmentDescriptorBase* descriptor_p) {
        RenderGraphAttachmentBase* res = nullptr;

        auto for_loop_body = [&]<RenderGraphAttachment AttachmentTypes>(AttachmentTypes attachmentTypes) {
            if (dynamic_cast<RenderGraphAttachmentDescriptor<AttachmentTypes>*>(descriptor_p) != nullptr) {
                auto obj = new RenderGraphAttachmentDerived<AttachmentTypes>();
                obj->descriptor_p = dynamic_cast<RenderGraphAttachmentDescriptor<AttachmentTypes>*>(descriptor_p);
                res = obj;
            }
        };

        auto for_loop = [&]<RenderGraphAttachment... AttachmentTypes>(TypeList<AttachmentTypes...>) {
            (for_loop_body(AttachmentTypes()), ...);
        };

        for_loop(RenderGraphAttachmentTypeList());

        return res;
    }

    [[nodiscard]] RenderGraphAttachmentBase* getAttachment(const std::string& name) const {
        for (auto att: attachments) {
            if (att->name == name) {
                return att;
            }
        }
        return nullptr;
    }

    [[nodiscard]] RenderGraphPassBase* getPass(const std::string& name) const {
        for (auto pass: passes){
            if (pass->name == name) {
                return pass;
            }
        }
        return nullptr;
    }

    explicit RenderGraph(VklDevice &device, RenderGraphDescriptor *renderGraphDescriptor, uint32_t instance_n): device_(device), instance_n_(instance_n) {
        renderGraphDescriptor_ = renderGraphDescriptor;

        for (auto & passDescriptor : renderGraphDescriptor->passDescriptors) {
            passes.push_back(pass_desc_to_obj(passDescriptor));
        }

        for (auto & attachmentDescriptor : renderGraphDescriptor->attachmentDescriptors) {
            attachments.push_back(attachment_desc_to_obj(attachmentDescriptor));
        }

        for (auto node: passes) {
            for (auto input: node->descriptor_p->input_descriptors) {
                RenderGraphAttachmentBase* att = getAttachment(input->name);
                node->input_attachments.push_back(att);
            }
            for (auto output: node->descriptor_p->output_descriptors) {
                RenderGraphAttachmentBase* att = getAttachment(output->name);
                node->output_attachments.push_back(att);
            }
        }
    }

    void createInstances() {
        // create render resources.

        for (int i = 0; i < instance_n_; i++) {
            for (auto edge: attachments) {
                // the edge is texture
                if (dynamic_cast<RenderGraphAttachmentDerived<RenderGraphTextureAttachment>*>(edge) != nullptr) {
                    auto texture_edge = dynamic_cast<RenderGraphAttachmentDerived<RenderGraphTextureAttachment>*>(edge);
                    texture_edge->instances[i]->texture = std::move(std::make_unique<VklTexture>(device_, texture_edge->descriptor_p->width, texture_edge->descriptor_p->height, 4));
                }

                // the edge is storage buffer
                if (dynamic_cast<RenderGraphAttachmentDerived<RenderGraphStorageBufferAttachment>*>(edge) != nullptr) {
                    throw std::runtime_error("storage buffer edge is not implemented");
                }
            }
        }
    }

    void render(VkCommandBuffer commandBuffer, uint32_t frame_index) {

    }

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
    struct Task: TaskBase {
        size_t TaskTypeIndex() override {
            return PassTypeIndex<T>();
        }
    };

    std::vector<TaskBase> tasks;

//    template<RenderGraphPass T>
//    void addPass(RenderGraphPassDescriptor<T> &&descriptor){
//        Task<T> task;
//        auto& descriptorVector = std::get<PassTypeIndex<RenderGraphRenderPass>()>(passDescriptors);
//        task.index = descriptor.size();
//        tasks.push_back(task);
//
//        descriptorVector.push_back(descriptor);
//    }

    void execute(VkCommandBuffer commandBuffer) {
//        for (auto task: tasks) {
//            if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphRenderPass>()) {
//                auto &descriptor = std::get<PassTypeIndex<RenderGraphRenderPass>()>(passDescriptors)[task.index];
//            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphComputePass>()) {
//                auto &descriptor = std::get<PassTypeIndex<RenderGraphComputePass>()>(passDescriptors)[task.index];
//            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphTransferPass>()) {
//                auto &descriptor = std::get<PassTypeIndex<RenderGraphTransferPass>()>(passDescriptors)[task.index];
//            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphImagePresentPass>()) {
//                auto &descriptor = std::get<PassTypeIndex<RenderGraphImagePresentPass>()>(passDescriptors)[task.index];
//            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphFrameSyncBeginPass>()) {
//                auto &descriptor = std::get<PassTypeIndex<RenderGraphFrameSyncBeginPass>()>(passDescriptors)[task.index];
//            } else if (task.TaskTypeIndex() == PassTypeIndex<RenderGraphFrameSyncEndPass>()) {
//                auto &descriptor = std::get<PassTypeIndex<RenderGraphFrameSyncEndPass>()>(passDescriptors)[task.index];
//            }
//        }
    }
};