#include "simulation/walk_on_sphere/walk_on_sphere_anisotropic.hpp"

#include "effolkronium/random.hpp"

#include "numbers"

using Random = effolkronium::random_static;

double AnisotropicWalkOnSphere::evaluate(glm::vec2 param) {
    double res = 0;
    const int iter = 4096;
    for (int i = 0; i < iter; i++) {
        res += evaluate_internal(param);
    }
    return res / iter;
}

double AnisotropicWalkOnSphere::sdf_evaluate(glm::vec2 param) {
    return std::min(param.x, std::min(param.y, std::min(1 - param.x, 1 - param.y)));
}

double AnisotropicWalkOnSphere::evaluate_internal(glm::vec2 param) {
    double sdf = sdf_evaluate(param);

    if (sdf < 1e-4)
        return boundary_evaluation(param);

    auto current_param = param;

    int count = 0;

    // Girsanov factor
    float exp_factor = 0.0;

    while(glm::length(current_param - param) < sdf - 1e-4) {
        count += 1;
        // auto diffusion = targetSurface->evaluate_laplacian_diffusion_coefficients(param);
        // auto drift = targetSurface->evaluate_laplacian_drift_coefficients(param);

        int idx1 = current_param.x / 0.01 + 1;
        int idx2 = current_param.y / 0.01 + 1;

        idx1 %= 100;
        idx2 %= 100;

        auto [diffusion, drift] = (*cache)[idx1][idx2];

        glm::mat2 sigma;

        sigma[0][0] = diffusion[0][0];
        sigma[0][1] = 0;
        sigma[1][0] = diffusion[0][1];
        sigma[1][1] = std::sqrt(diffusion[0][0] * diffusion[1][1] - diffusion[0][1] * diffusion[1][0]);
        sigma /= std::sqrt(diffusion[0][0] / 2);

        glm::mat2 sigma_inverse(0.0f);
        sigma_inverse[0][0] = sigma[1][1];
        sigma_inverse[1][1] = sigma[0][0];
        sigma_inverse[0][1] = -sigma[1][0];
        sigma_inverse[1][0] = -sigma[0][1];

        float sigma_det = glm::determinant(sigma);
        sigma_inverse = sigma_inverse / sigma_det;

        float time_step = 0.01;
        auto b1 = Random::get<std::normal_distribution<>>(0.0, 1.0);
        auto b2 = Random::get<std::normal_distribution<>>(0.0, 1.0);

        glm::vec2 dB(b1, b2);
        dB = dB * std::sqrt(time_step);

        auto b1_prime = Random::get<std::normal_distribution<>>(0.0, 1.0);
        auto b2_prime = Random::get<std::normal_distribution<>>(0.0, 1.0);

        glm::vec2 dB_prime(b1_prime, b2_prime);
        dB_prime = dB_prime * std::sqrt(time_step);

        glm::vec2 dt(time_step);

        glm::vec2 u = sigma_inverse * drift;

        float dZ = 0.0;
        dZ -= glm::dot(u, dB_prime);
        dZ -= 0.5f * glm::dot(u, u) * time_step;

        exp_factor += dZ;

        auto delta =  sigma * dB;

        current_param = current_param + delta;

        if (count > 1000) break;
    }

    if (count > 1000)
        std::cout << "failed" << std::endl;

    auto dir = glm::normalize(current_param - param);

    glm::vec2 r = dir * float(sdf);

    return evaluate_internal(param + r) * exp(exp_factor);
}

double AnisotropicWalkOnSphere::boundary_evaluation(glm::vec2 param) {
    std::vector<double> dist = {param.x, param.y, 1 - param.x, 1 - param.y};

    size_t idx = 0;
    double minValue = param.x;

    for (auto i = 1; i < dist.size(); i++) {
        if (dist[i] < minValue) {
            minValue = dist[i];
            idx = i;
        }
    }

    if (idx == 0)
        param.x = 0;
    if (idx == 1)
        param.y = 0;
    if (idx == 2)
        param.x = 1;
    if (idx == 3)
        param.y = 1;

    auto pos = targetSurface->evaluate(param);

    return pos.z;
}

TensorProductBezierSurface::render_type::BuilderFromImmediateData AnisotropicWalkOnSphere::getMeshModelBuilderWos() {
    TensorProductBezierSurface::render_type::BuilderFromImmediateData builder;

    constexpr int n = 50;
    constexpr int m = 50;

    float delta_u = 1.0f / n;
    float delta_v = 1.0f / m;

    float minValue = 1000;
    float maxValue = -1000;

    cache = std::make_unique<cache_data_type>();

    for (int i = 0; i < 100; i++) {
        std::cout << "pre compute " << i << std::endl;
        for (int j = 0; j < 100; j++) {
            glm::vec2 param{i * 0.01, j * 0.01};
            auto diffusion = targetSurface->evaluate_laplacian_diffusion_coefficients(param);
            auto drift = targetSurface->evaluate_laplacian_drift_coefficients(param);
            (*cache)[i][j] = std::make_tuple(diffusion, drift);
        }
    }

    builder.vertices.resize((m + 1) * (n + 1));

    // #pragma omp parallel for num_threads(8)
    for (int i = 0; i <= m; i++) {
        std::cout << "test " << i << std::endl;
        for (int j = 0; j <= n; j++) {
            glm::vec2 param{delta_u * i, delta_v * j};
            auto position = targetSurface->evaluate(param);
            decltype(builder.vertices)::value_type vertex;
            vertex.position = position;

            double res = evaluate(param);

            glm::clamp(res, -1.0, 1.0);
            vertex.color = {res, 0.0, 0.0};

            builder.vertices[i * (n + 1) + j] = vertex;
        }
    }

    for (auto &vertex: builder.vertices) {
        minValue = std::min(minValue, vertex.color.x);
        maxValue = std::max(maxValue, vertex.color.x);
    }

    for (auto &vertex : builder.vertices) {
        vertex.color = (vertex.color - glm::vec3{float(-1), 0.0f, 0.0f}) / float(2);
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