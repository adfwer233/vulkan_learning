#pragma once

#include "vkl/core/vkl_buffer.hpp"
#include "vkl/core/vkl_descriptor.hpp"
#include <vector>

#include "glm/ext/matrix_transform.hpp"

class VklBox3D {
  public:
    glm::vec3 min_position;
    glm::vec3 max_position;

    void apply_transform(glm::mat4 trans) {
        float dx = max_position.x - min_position.x;
        float dy = max_position.y - min_position.y;
        float dz = max_position.z - min_position.z;

        std::vector<glm::vec3> points = {
            min_position + glm::vec3{dx, dy, dz}, min_position + glm::vec3{dx, dy, 0},
            min_position + glm::vec3{dx, 0, dz},  min_position + glm::vec3{dx, 0, 0},
            min_position + glm::vec3{0, dy, dz},  min_position + glm::vec3{0, dy, 0},
            min_position + glm::vec3{0, 0, dz},   min_position + glm::vec3{0, 0, 0},
        };

        min_position = glm::vec3(999999.0f);
        max_position = glm::vec3(-999999.0f);

        for (auto &pt : points) {
            auto pt_trans = trans * glm::vec4(pt, 1.0f);
            min_position.x = std::min(min_position.x, pt_trans.x);
            min_position.y = std::min(min_position.y, pt_trans.y);
            min_position.z = std::min(min_position.z, pt_trans.z);
            max_position.x = std::max(max_position.x, pt_trans.x);
            max_position.y = std::max(max_position.y, pt_trans.y);
            max_position.z = std::max(max_position.z, pt_trans.z);
        }
    }

    glm::mat4 get_box_transformation() {
        glm::mat4 trans(1.0f);
        trans = glm::translate(trans, min_position);
        trans = glm::scale(trans, max_position - min_position);
        return trans;
    }
};

class VklBox2D {
  public:
    glm::vec2 min_position;
    glm::vec2 max_position;

    void apply_transform(glm::mat3 trans) {
        float dx = max_position.x - min_position.x;
        float dy = max_position.y - min_position.y;

        std::vector<glm::vec2> points = {
            min_position + glm::vec2{dx, dy},
            min_position + glm::vec2{dx, 0},
            min_position + glm::vec2{0, dy},
            min_position + glm::vec2{0, 0},
        };

        min_position = glm::vec2(999999.0f);
        max_position = glm::vec2(-999999.0f);

        for (auto &pt : points) {
            auto pt_trans = trans * glm::vec3(pt, 1.0f);
            min_position.x = std::max(min_position.x, pt_trans.x);
            min_position.y = std::max(min_position.y, pt_trans.y);
            max_position.x = std::min(max_position.x, pt_trans.x);
            max_position.y = std::min(max_position.y, pt_trans.y);
        }
    }
};