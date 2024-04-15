#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "vkl/scene/vkl_model.hpp"

#include "geometry/autodiff/autodiff.hpp"
#include "surface.hpp"

#include "geometry/parameter_space/bezier_curve_2d.hpp"

/**
 * @brief Tensor Product Bezier Surface with automatic differentiation
 */
class TensorProductBezierSurface : GeometrySurface {
  private:
    /**
     * control points of the tensor product bezier surface
     */
    std::vector<std::vector<std::array<float, 3>>> control_points_;

    /**
     * boundary curves of the geometry surface
     */
    std::vector<std::unique_ptr<BezierCurve2D>> boundary_curves;

  public:
    explicit TensorProductBezierSurface(decltype(control_points_) &&control_pts) : control_points_(control_pts) {
        std::vector<BezierCurve2D::point_type> default_boundary1{BezierCurve2D::point_type{0.0, 0.0},
                                                                 BezierCurve2D::point_type{1.0, 0.0}};
        boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary1))));

        std::vector<BezierCurve2D::point_type> default_boundary2{BezierCurve2D::point_type{1.0, 0.0},
                                                                 BezierCurve2D::point_type{1.0, 1.0}};
        boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary2))));

        std::vector<BezierCurve2D::point_type> default_boundary3{BezierCurve2D::point_type{1.0, 1.0},
                                                                 BezierCurve2D::point_type{0.0, 1.0}};
        boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary3))));

        std::vector<BezierCurve2D::point_type> default_boundary4{BezierCurve2D::point_type{0.0, 1.0},
                                                                 BezierCurve2D::point_type{0.0, 0.0}};
        boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary4))));
    }

    explicit TensorProductBezierSurface(std::vector<std::vector<glm::vec3>> &&control_pts) {
        for (const auto &item_i : control_pts) {
            auto &target = control_points_.emplace_back();
            for (auto item_j : item_i) {
                target.emplace_back(std::array<float, 3>{item_j.x, item_j.y, item_j.z});
            }
        }

        std::vector<BezierCurve2D::point_type> default_boundary1{BezierCurve2D::point_type{0.0, 0.0},
                                                                 BezierCurve2D::point_type{1.0, 0.0}};
        boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary1))));

        std::vector<BezierCurve2D::point_type> default_boundary2{BezierCurve2D::point_type{1.0, 0.0},
                                                                 BezierCurve2D::point_type{1.0, 1.0}};
        boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary2))));

        std::vector<BezierCurve2D::point_type> default_boundary3{BezierCurve2D::point_type{1.0, 1.0},
                                                                 BezierCurve2D::point_type{0.0, 1.0}};
        boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary3))));

        std::vector<BezierCurve2D::point_type> default_boundary4{BezierCurve2D::point_type{0.0, 1.0},
                                                                 BezierCurve2D::point_type{0.0, 0.0}};
        boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary4))));
    }
    /**
     * evaluate the position with given parameter.
     * @param param vec2 in the parameter space.
     * @return position in the physical space.
     */
    glm::vec3 evaluate(glm::vec2 param);

    /**
     * evaluate the position, automatic differential version
     * @param param autodiff vec2, parameter
     */
    autodiff_vec3 evaluate_autodiff(autodiff_vec2 &param);

    /**
     * evaluate the metric tensor at given position
     */
    glm::mat2 evaluate_metric_tensor(glm::vec2 param);

    glm::vec3 evaluate_ru(glm::vec2 param);

    /**
     * evaluate the metric tensor at given position, autodiff version
     * @param param
     * @return
     */
    autodiff_mat2 evaluate_metric_tensor_autodiff(autodiff_vec2 &param);

    /**
     * evaluate the determinant of the  metric tensor at given position
     * @param param
     * @return
     */
    double evaluate_det_metric_tensor(glm::vec2 param);

    /**
     * evaluate the determinant of the metric tensor at given position, autodiff version
     * @param param
     * @return
     */
    autodiff::var evaluate_det_metric_tensor_autodiff(autodiff_vec2 &param);

    /**
     * evaluate the inverse metric tensor g^{ij}
     * @param param
     * @return
     */
    glm::mat2 evaluate_inverse_metric_tensor(glm::vec2 param);

    /**
     * evaluate the inverse metric tensor g^{ij}, autodiff version
     * @param param
     * @return
     */
    autodiff_mat2 evaluate_inverse_metric_tensor_autodiff(autodiff_vec2 &param);

    /**
     * evaluate the diffusion coefficients of the laplacian beltrami operator at given position
     * @param param
     * @return
     */
    glm::mat2 evaluate_laplacian_diffusion_coefficients(glm::vec2 param);

    /**
     * evaluate the drift coefficient of the laplacian beltrami operator at given position
     * @param param
     * @return
     */
    glm::vec2 evaluate_laplacian_drift_coefficients(glm::vec2 param);

    /**
     * implementing the RenderableGeometry concept
     */
    struct IsRenderableGeometry {};
    using render_type = VklModel;
    using boundary_render_type = BezierCurve2D::render_type;

    render_type *getMeshModel(VklDevice &device) {
        if (mesh_model_ptr) {
            return mesh_model_ptr.get();
        }

        auto builder = getMeshModelBuilder();
        mesh_model_ptr = std::make_unique<render_type>(device, builder);
        return mesh_model_ptr.get();
    }

    decltype(auto) getBoundaryMeshModels(VklDevice &device) {
        std::vector<boundary_render_type *> result;
        if (not boundary_curves_ptr.empty()) {
            for (auto &ptr : boundary_curves_ptr) {
                result.push_back(ptr.get());
            }
        } else {
            for (auto &boundary : boundary_curves) {
                auto parameter_space_mesh_model_ptr = boundary->get_parameter_space_mesh_model(device);
                boundary_render_type ::BuilderFromImmediateData builder;

                for (auto &param_vert : parameter_space_mesh_model_ptr->vertices_) {
                    boundary_render_type ::vertex_type vertex;
                    auto position = evaluate(param_vert.position);

                    vertex.position = position;
                    vertex.color = {1.0, 0.0, 0.0};

                    builder.vertices.push_back(vertex);
                }

                boundary_curves_ptr.push_back(std::move(std::make_unique<boundary_render_type>(device, builder)));
                result.push_back(boundary_curves_ptr.back().get());
            }
        }
        return result;
    }

    render_type::BuilderFromImmediateData getMeshModelBuilder() {
        render_type::BuilderFromImmediateData builder;

        constexpr int n = 50;
        constexpr int m = 50;

        float delta_u = 1.0f / n;
        float delta_v = 1.0f / m;

        for (int i = 0; i <= m; i++) {
            for (int j = 0; j <= n; j++) {
                glm::vec2 param{delta_u * i, delta_v * j};
                auto position = evaluate(param);
                decltype(builder.vertices)::value_type vertex;
                vertex.position = position;
                builder.vertices.push_back(vertex);
            }
        }

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                auto idx1 = i * (n + 1) + j;
                auto idx2 = i * (n + 1) + j + 1;
                auto idx3 = (i + 1) * (n + 1) + j;
                auto idx4 = (i + 1) * (n + 1) + j + 1;

                decltype(builder.indices)::value_type primitive_idx1, primitive_idx2;
                primitive_idx1.i = idx1;
                primitive_idx1.j = idx2;
                primitive_idx1.k = idx4;

                primitive_idx2.i = idx1;
                primitive_idx2.j = idx4;
                primitive_idx2.k = idx3;

                builder.indices.push_back(primitive_idx1);
                builder.indices.push_back(primitive_idx2);
            }
        }

        return builder;
    }

    virtual GeometrySurfaceType type() {
        return GeometrySurfaceType::TensorProductBezier;
    };

  private:
    std::unique_ptr<render_type> mesh_model_ptr;
    std::vector<std::unique_ptr<boundary_render_type>> boundary_curves_ptr;
};