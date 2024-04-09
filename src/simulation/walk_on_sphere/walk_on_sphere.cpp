#include "simulation/walk_on_sphere/walk_on_sphere.hpp"

#include "effolkronium/random.hpp"

#include <numbers>

using Random = effolkronium::random_static;

double ParameterSpaceWalkOnSphere::evaluate(glm::vec2 param) {
    double res = 0;
    const int iter = 1024;
    for (int i = 0; i < iter; i++)
        res += evaluate_internal(param);
    return res / iter;
}

double ParameterSpaceWalkOnSphere::sdf_evaluate(glm::vec2 param) {
    return std::min(param.x, std::min(param.y, std::min(1 - param.x, 1 - param.y)));
}

double ParameterSpaceWalkOnSphere::evaluate_internal(glm::vec2 param) {
    double sdf = sdf_evaluate(param);

    if (sdf < 1e-4)
        return boundary_evaluation(param);

    auto theta = Random::get(0.0, 2 * std::numbers::pi);

    glm::vec2 r = {sdf * std::cos(theta), sdf * std::sin(theta)};
    return evaluate_internal(param + r);
}

double ParameterSpaceWalkOnSphere::boundary_evaluation(glm::vec2 param) {
    std::vector<double> dist = {param.x, param.y, 1 - param.x, 1 - param.y};

    size_t idx = 0;
    double minValue = param.x;

    for (auto i = 1; i < dist.size(); i++) {
        if (dist[i] < minValue) {
            minValue = dist[i];
            idx = i;
        }
    }

    if (idx == 0) param.x = 0;
    if (idx == 1) param.y = 0;
    if (idx == 2) param.x = 1;
    if (idx == 3) param.y = 1;

    auto pos = targetSurface->evaluate(param);

    return pos.z;
}
TensorProductBezierSurface::render_type::BuilderFromImmediateData ParameterSpaceWalkOnSphere::getMeshModelBuilderWos() {
    TensorProductBezierSurface::render_type::BuilderFromImmediateData builder;

    constexpr int n = 50;
    constexpr int m = 50;

    float delta_u = 1.0f / n;
    float delta_v = 1.0f / m;

    double minValue = 1000;
    double maxValue = -1000;

    builder.vertices.resize((m + 1) * (n + 1));

//#pragma omp parallel for num_threads(8)
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            glm::vec2 param{delta_u * i, delta_v * j};
            auto position = targetSurface->evaluate(param);
            decltype(builder.vertices)::value_type vertex;
            vertex.position = position;

            double res = evaluate(param);
            minValue = std::min(res, minValue);
            maxValue = std::max(res, maxValue);

            vertex.color = {res, 0.0, 0.0};

            builder.vertices[i * (n + 1) + j] = vertex;
        }
    }

    for (auto &vertex: builder.vertices) {
        vertex.color = (vertex.color - glm::vec3{float(minValue), 0.0f, 0.0f}) / float(maxValue - minValue);
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
