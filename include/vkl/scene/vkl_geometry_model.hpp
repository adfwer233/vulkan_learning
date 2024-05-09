#pragma once

#include "geometry/surface/tensor_product_bezier.hpp"
#include "vkl/utils/vkl_curve_model.hpp"
#include "vkl_model.hpp"
#include <vector>
#include <numbers>

template <typename T> class VklGeometryModel {};

template <typename T> class VklGeometryModelDescriptorManager {
public:
    std::unique_ptr<VklDescriptorSetLayout> setLayout_;
    std::unique_ptr<VklDescriptorPool> descriptorPool_;

    static VklGeometryModelDescriptorManager<T> *instance(VklDevice &device) {
        if (instance_ == nullptr) {
            instance_ = new VklGeometryModelDescriptorManager<T>(device);
        }
        return instance_;
    }

private:
    static inline VklGeometryModelDescriptorManager<T> *instance_ = nullptr;

    explicit VklGeometryModelDescriptorManager<T>(VklDevice &device) {
        setLayout_ = VklDescriptorSetLayout::Builder(device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

        descriptorPool_ = VklDescriptorPool::Builder(device)
                .setMaxSets(VklSwapChain::MAX_FRAMES_IN_FLIGHT * 2000)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VklSwapChain::MAX_FRAMES_IN_FLIGHT * 2000)
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VklSwapChain::MAX_FRAMES_IN_FLIGHT * 2000)
                .build();
    }
};

template <typename T> class VklGeometryModelBuffer {
  public:
    VklGeometryModel<T> *getGeometryModel(VklDevice &device, T *ptr) {
        if (map_.contains(ptr))
            return map_[ptr];
        else {
            map_[ptr] = new VklGeometryModel<T>(device, ptr);
            return map_[ptr];
        }
    }

    static VklGeometryModelBuffer<T> *instance() {
        if (instance_ == nullptr) {
            instance_ = new VklGeometryModelBuffer<T>();
        }
        return instance_;
    }

    ~VklGeometryModelBuffer() {
        for (auto &[k, v] : map_) {
            delete v;
        }
    }

  private:
    std::map<T *, VklGeometryModel<T> *> map_;
    static inline VklGeometryModelBuffer<T> *instance_ = nullptr;
    // VklGeometryModelBuffer<T>() = default;
};

template <> class VklGeometryModel<BezierCurve2D> {
public:
    BezierCurve2D *curve_;
    VklDevice &device_;

    using render_type = VklCurveModel2D;
    using control_points_render_type = VklPointCloud2D;
    using extreme_point_render_type = VklPointCloud2D;
    using derivative_bound_render_type = VklCurveModel2D;

    std::unique_ptr<render_type> curveMesh;
    std::unique_ptr<control_points_render_type> controlPointsMesh;
    std::unique_ptr<extreme_point_render_type> extremePointMesh;
    std::unique_ptr<derivative_bound_render_type> derivativeBoundMesh;

    VklGeometryModel(VklDevice &device, BezierCurve2D *curve): device_(device), curve_(curve) {
        createControlPointsMesh();

        if (curve->control_point_vec2.size() >= 3)
            createMesh();
    }

    void reallocateMesh() {
        if (curveMesh == nullptr and curve_->control_point_vec2.size() >= 3) {
            createMesh();
            createDerivativeBoundMesh();
        }

        if (extremePointMesh == nullptr and curve_->control_point_vec2.size() >= 3) {
            createExtremePointMesh();
        }

        if (curveMesh != nullptr) {
            std::vector<render_type::vertex_type::geometry_type> vertices;
            constexpr int n = 100;
            double param_delta = 1.0 / n;

            for (int i = 0; i <= n; i++) {
                render_type::vertex_type vertex;
                auto position = curve_->evaluate(param_delta * i);
                vertex.position = position;
                vertex.color = {1.0, 0.0, 0.0};
                vertices.push_back(vertex);
            }

            curveMesh->geometry->vertices = vertices;
            curveMesh->reallocateVertexBuffer();
        }

        std::vector<control_points_render_type::vertex_type::geometry_type> control_vertices;
        for (auto cp: curve_->control_point_vec2) {
            control_points_render_type::vertex_type vertex;
            vertex.position = cp;
            control_vertices.push_back(vertex);
        }
        controlPointsMesh->geometry->vertices = control_vertices;
        controlPointsMesh->reallocateVertexBuffer();

        auto vkl_vertex_to_geo_vertex = []<typename T>(T geo_vert) {
            typename T::geometry_type vert;
            vert.position = geo_vert.position;
            vert.color = geo_vert.color;
            return vert;
        };

        if (derivativeBoundMesh != nullptr) {
            auto derivativeBuilder = createDerivativeBoundMeshBuilder();
            std::vector<control_points_render_type::vertex_type::geometry_type> derivative_vertices;
            for (auto geo_vert: derivativeBuilder.vertices) {
                derivative_bound_render_type::vertex_type vertex;
                vertex.position = geo_vert.position;
                derivative_vertices.push_back(vertex);
            }

            auto res = derivativeBuilder.vertices | std::views::transform(vkl_vertex_to_geo_vertex);
            derivativeBoundMesh->geometry->vertices.clear();
            std::ranges::copy(res, std::back_inserter(derivativeBoundMesh->geometry->vertices));

            derivativeBoundMesh->reallocateVertexBuffer();
        }

        if (extremePointMesh != nullptr) {
            auto extremePointBuilder = createExtremePointMeshBuilder();

            extremePointMesh->geometry->vertices.clear();
            std::ranges::copy(extremePointBuilder.vertices | std::views::transform(vkl_vertex_to_geo_vertex), std::back_inserter(extremePointMesh->geometry->vertices));

            extremePointMesh->reallocateVertexBuffer();
        }
    }
private:

    extreme_point_render_type::BuilderFromImmediateData createExtremePointMeshBuilder() {
        auto ex_points = curve_->compute_extreme_points();
        extreme_point_render_type::BuilderFromImmediateData builder;
        for (auto ep: ex_points) {
            extreme_point_render_type::vertex_type vertex;
            vertex.position = ep;
            vertex.color = {0.0, 1.0, 0.0};
            builder.vertices.push_back(vertex);
        }
        return builder;
    }

    void createExtremePointMesh() {
        auto builder = createExtremePointMeshBuilder();
        if (builder.vertices.empty()) {
            extremePointMesh = nullptr;
            return;
        }
        extremePointMesh = std::make_unique<extreme_point_render_type>(device_, builder);
        auto descriptorManager = VklGeometryModelDescriptorManager<BezierCurve2D>::instance(device_);
        extremePointMesh->allocDescriptorSets(*descriptorManager->setLayout_, *descriptorManager->descriptorPool_);
    }

    derivative_bound_render_type::BuilderFromImmediateData createDerivativeBoundMeshBuilder() {
        auto start_pos = curve_->control_point_vec2.front();
        auto end_pos = curve_->control_point_vec2.back();

        float eps = 1e-3;

        derivative_bound_render_type::BuilderFromImmediateData builder;
        if (glm::length(end_pos - start_pos) < eps) {
            // the boundary is a circle
            auto center = (start_pos + end_pos) / 2.0f;
            auto bound = curve_->derivative_bound;

            int n = 100;

            for (int i = 0; i <= n; i++) {
                float theta = 2.0 * std::numbers::pi / n * i;
                derivative_bound_render_type::vertex_type vertex;
                vertex.position.x = center.x + bound / 2.0f * std::cos(theta);
                vertex.position.y = center.y + bound / 2.0f * std::sin(theta);
                vertex.color = {0.0, 1.0, 0.0};
                builder.vertices.push_back(vertex);
            }
        } else {
            // the boundary is an ellipse
            float a = curve_->derivative_bound / 2;
            float c = glm::length(end_pos - start_pos) / 2;
            float b = std::sqrt(a * a - c * c);

            int n = 100;

            glm::vec2 center = (start_pos + end_pos) / 2.0f;
            glm::vec2 x_axis = glm::normalize((end_pos - start_pos) / 2.0f);
            glm::vec2 y_axis = {x_axis.y, -x_axis.x};

            for (int i = 0; i <= n; i++) {
                float theta = 2.0 * std::numbers::pi / n * i;
                derivative_bound_render_type::vertex_type vertex;
                float x_coord = a * std::cos(theta);
                float y_coord = b * std::sin(theta);

                vertex.position = center + x_axis * x_coord + y_axis * y_coord;
                vertex.color = {0.0, 1.0, 0.0};

                builder.vertices.push_back(vertex);
            }
        }

        return builder;
    }

    void createDerivativeBoundMesh() {
        auto builder = createDerivativeBoundMeshBuilder();

        derivativeBoundMesh = std::make_unique<derivative_bound_render_type>(device_, builder);

        auto descriptorManager = VklGeometryModelDescriptorManager<BezierCurve2D>::instance(device_);
        derivativeBoundMesh->allocDescriptorSets(*descriptorManager->setLayout_, *descriptorManager->descriptorPool_);
    }

    void createControlPointsMesh() {
        control_points_render_type::BuilderFromImmediateData builder;
        for (auto cp: curve_->control_point_vec2) {
            control_points_render_type::vertex_type vertex;
            vertex.position = cp;
            builder.vertices.push_back(vertex);
        }
        controlPointsMesh = std::make_unique<control_points_render_type>(device_, builder);

        auto descriptorManager = VklGeometryModelDescriptorManager<BezierCurve2D>::instance(device_);
        controlPointsMesh->allocDescriptorSets(*descriptorManager->setLayout_, *descriptorManager->descriptorPool_);
    }

    void createMesh() {
        render_type::BuilderFromImmediateData builder;
        constexpr int n = 100;
        double param_delta = 1.0 / n;

        for (int i = 0; i <= n; i++) {
            render_type::vertex_type vertex;
            auto position = curve_->evaluate(param_delta * i);
            vertex.position = position;
            vertex.color = {1.0, 0.0, 0.0};
            builder.vertices.push_back(vertex);
        }

        curveMesh = std::make_unique<render_type>(device_, builder);

        auto descriptorManager = VklGeometryModelDescriptorManager<BezierCurve2D>::instance(device_);
        curveMesh->allocDescriptorSets(*descriptorManager->setLayout_,
                                                *descriptorManager->descriptorPool_);
    }
};

template <> class VklGeometryModel<TensorProductBezierSurface> {
  public:
    TensorProductBezierSurface *surface_;
    VklDevice &device_;

    using boundary_render_type = VklCurveModel3D;
    using parameter_render_type = VklCurveModel2D;

    std::vector<std::unique_ptr<boundary_render_type>> boundary_3d;
    std::vector<std::unique_ptr<parameter_render_type>> boundary_2d;

    VklGeometryModel(VklDevice &device, TensorProductBezierSurface *surf) : device_(device), surface_(surf) {
        createBoundaryModels();
        createParameterBoundaryModels();
    }

  private:
    void createBoundaryModels() {
        constexpr int n = 100;
        double param_delta = 1.0 / n;

        for (auto &boundary : surface_->boundary_curves) {
            boundary_render_type::BuilderFromImmediateData builder;
            for (int i = 0; i <= n; i++) {
                boundary_render_type::vertex_type vertex;
                auto position = surface_->evaluate(boundary->evaluate(param_delta * i));
                vertex.position = position;
                builder.vertices.push_back(vertex);
            }
            boundary_3d.push_back(std::move(std::make_unique<boundary_render_type>(device_, builder)));

            auto descriptorManager = VklGeometryModelDescriptorManager<TensorProductBezierSurface>::instance(device_);
            boundary_3d.back()->allocDescriptorSets(*descriptorManager->setLayout_,
                                                    *descriptorManager->descriptorPool_);
        }
    }

    void createParameterBoundaryModels() {
        constexpr int n = 100;
        double param_delta = 1.0 / n;
        for (auto &boundary : surface_->boundary_curves) {
            parameter_render_type::BuilderFromImmediateData builder;
            for (int i = 0; i <= n; i++) {
                parameter_render_type::vertex_type vertex;
                auto position = boundary->evaluate(param_delta * i);
                vertex.position = position;
                vertex.color = {1.0, 0.0, 0.0};
                builder.vertices.push_back(vertex);
            }
            boundary_2d.push_back(std::move(std::make_unique<parameter_render_type>(device_, builder)));

            auto descriptorManager = VklGeometryModelDescriptorManager<TensorProductBezierSurface>::instance(device_);
            boundary_2d.back()->allocDescriptorSets(*descriptorManager->setLayout_,
                                                    *descriptorManager->descriptorPool_);
        }
    };
};