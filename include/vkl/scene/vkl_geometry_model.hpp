#pragma once

#include <vector>
#include "vkl_model.hpp"
#include "vkl/utils/vkl_curve_model.hpp"
#include "geometry/surface/tensor_product_bezier.hpp"

template<typename T>
class VklGeometryModel {};

template<typename T>
class VklGeometryModelDescriptorManager {};

template<> class VklGeometryModelDescriptorManager<TensorProductBezierSurface> {
public:
    std::unique_ptr<VklDescriptorSetLayout> setLayout_;
    std::unique_ptr<VklDescriptorPool> descriptorPool_;

    static VklGeometryModelDescriptorManager<TensorProductBezierSurface>* instance(VklDevice &device) {
        if (instance_ == nullptr) {
            instance_ = new VklGeometryModelDescriptorManager<TensorProductBezierSurface>(device);
        }
        return instance_;
    }
private:
    static inline VklGeometryModelDescriptorManager<TensorProductBezierSurface>* instance_ = nullptr;

    explicit VklGeometryModelDescriptorManager<TensorProductBezierSurface>(VklDevice &device) {
        setLayout_ = VklDescriptorSetLayout::Builder(device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

        descriptorPool_ = VklDescriptorPool::Builder(device)
                .setMaxSets(VklSwapChain::MAX_FRAMES_IN_FLIGHT * 200)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VklSwapChain::MAX_FRAMES_IN_FLIGHT * 200)
                .build();
    }
};

template<typename T>
class VklGeometryModelBuffer {
public:
    VklGeometryModel<T>* getGeometryModel (VklDevice &device, T* ptr)  {
        if (map_.contains(ptr))
            return map_[ptr];
        else {
            map_[ptr] = new VklGeometryModel<T>(device, ptr);
            return map_[ptr];
        }
    }

    static VklGeometryModelBuffer<T>* instance() {
        if (instance_ == nullptr) {
            instance_ = new VklGeometryModelBuffer<T>();
        }
        return instance_;
    }

    ~VklGeometryModelBuffer() {
        for (auto& [k, v]: map_) {
            delete v;
        }
    }
private:
    std::map<T*, VklGeometryModel<T>*> map_;
    static inline VklGeometryModelBuffer<T> *instance_ = nullptr;
    VklGeometryModelBuffer<T>() = default;
};

template<> class VklGeometryModel<TensorProductBezierSurface> {
public:
    TensorProductBezierSurface *surface_;
    VklDevice &device_;

    using boundary_render_type = VklCurveModel3D;
    using parameter_render_type = VklCurveModel2D;

    std::vector<std::unique_ptr<boundary_render_type>> boundary_3d;
    std::vector<std::unique_ptr<parameter_render_type>> boundary_2d;

    VklGeometryModel<TensorProductBezierSurface>(VklDevice &device, TensorProductBezierSurface *surf) : device_(device), surface_(surf) {
        createBoundaryModels();
        createParameterBoundaryModels();
    }

private:
    void createBoundaryModels() {
        constexpr int n = 100;
        double param_delta = 1.0 / n;

        for (auto &boundary: surface_->boundary_curves) {
            boundary_render_type::BuilderFromImmediateData builder;
            for (int i = 0; i <= n; i++) {
                boundary_render_type::vertex_type vertex;
                auto position = surface_->evaluate(boundary->evaluate(param_delta * i));
                vertex.position = position;
                builder.vertices.push_back(vertex);
            }
            boundary_3d.push_back(std::move(std::make_unique<boundary_render_type >(device_, builder)));

            auto descriptorManager = VklGeometryModelDescriptorManager<TensorProductBezierSurface>::instance(device_);
            boundary_3d.back()->allocDescriptorSets(*descriptorManager->setLayout_, *descriptorManager->descriptorPool_);
        }
    }

    void createParameterBoundaryModels() {
        constexpr int n = 100;
        double param_delta = 1.0 / n;
        for (auto &boundary: surface_->boundary_curves) {
            parameter_render_type::BuilderFromImmediateData builder;
            for (int i = 0; i <= n; i++) {
                parameter_render_type::vertex_type vertex;
                auto position = boundary->evaluate(param_delta * i);
                vertex.position = position;
                builder.vertices.push_back(vertex);
            }
            boundary_2d.push_back(std::move(std::make_unique<parameter_render_type>(device_, builder)));

            auto descriptorManager = VklGeometryModelDescriptorManager<TensorProductBezierSurface>::instance(device_);
            boundary_2d.back()->allocDescriptorSets(*descriptorManager->setLayout_, *descriptorManager->descriptorPool_);
        }
    };

};