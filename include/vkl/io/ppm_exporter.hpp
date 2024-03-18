#pragma once

#include <array>
#include <fstream>

#include "glm/glm.hpp"

template <size_t n, size_t m> void output_ppm_image(const std::array<std::array<glm::vec3, m>, n> &image) {
    std::ofstream file("render.ppm", std::ios::out);

    file << "P3\n" << m << ' ' << n << "\n255\n";

    for (auto i = 0; i < n; i++) {
        for (auto j = 0; j < m; j++) {
            auto pixel = image[i][j];
            pixel.x *= 255.99;
            pixel.y *= 255.99;
            pixel.z *= 255.99;
            file << int(pixel.x) << ' ' << int(pixel.y) << ' ' << int(pixel.z) << std::endl;
        }
    }
    file.close();
}

template <size_t n, size_t m> void output_ppm_image(const std::array<std::array<glm::vec4, m>, n> &image) {
    std::ofstream file("render.ppm", std::ios::out);

    file << "P3\n" << m << ' ' << n << "\n255\n";

    for (auto i = 0; i < n; i++) {
        for (auto j = 0; j < m; j++) {
            auto pixel = image[i][j];
            pixel.x *= 255.99;
            pixel.y *= 255.99;
            pixel.z *= 255.99;
            file << int(pixel.x) << ' ' << int(pixel.y) << ' ' << int(pixel.z) << std::endl;
        }
    }
    file.close();
}