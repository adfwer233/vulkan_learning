#pragma once

#include <array>
#include <optional>
#include <stack>

#include "vkl/io/ppm_exporter.hpp"

#include "vkl_ray.hpp"
#include "vkl/vkl_scene.hpp"
#include "vkl/bvh/vkl_bvh.hpp"

const int n = 1024;
const int m = 1024;
std::array<std::array<glm::vec4, m>, n> output;

struct HitRecord {
    glm::vec3 position;
    glm::vec3 normal;
    uint32_t materialIndex;
    float t;
};

class VklCpuRayTracer {

private:
    VklScene &scene_;
    VklBVH bvh_;

    std::vector<VklBVHGPUModel::BVHNode> bvhTree;

    /**
     * @brief intersection ray and AABB
     *
     * Ref to: A Ray-Box Intersection Algorithm and Efficient Dynamic Voxel Rendering
     *
     * tNear > tFar =======> no intersection
     *
     * @param ray
     * @param min
     * @param max
     * @return (tNear, tFar)
     */
    glm::vec2 intersectAABB(VklRay ray, glm::vec3 &min, glm::vec3 &max) {
        glm::vec3 tMin = (min - ray.base) / ray.dir;
        glm::vec3 tMax = (max - ray.base) / ray.dir;
        glm::vec3 t1 = glm::min(tMin, tMax);
        glm::vec3 t2 = glm::max(tMin, tMax);
        float tNear = std::max(std::max(t1.x, t1.y), t1.z);
        float tFar = std::min(std::min(t2.x, t2.y), t2.z);

        return {tNear, tFar};
    }

    /**
     * @brief intersect between a ray and a triangle
     *
     * @param ray
     * @param tri
     * @param n normal vector of tir, returned as a reference
     * @return (t, u, v) param of ray -> t, first two barycentric coordinate on triangle -> u, v
     */
    glm::vec3 triangleIntersection(VklRay &ray, VklBVHGPUModel::Triangle tri, glm::vec3 &n) {
        glm::vec3 a = tri.v0 - tri.v1;
        glm::vec3 b = tri.v2 - tri.v0;
        glm::vec3 p = tri.v0 - ray.base;

        n = glm::cross(b, a);

        glm::vec3 q = glm::cross(p, ray.dir);
        float idet = 1.0f / glm::dot(ray.dir, n);

        float u = glm::dot(q, b) * idet;
        float v = glm::dot(q, a) * idet;
        float t = glm::dot(n, p) * idet;

        return {t, u, v};
    }

    std::optional<HitRecord> hitTriangle(int objectIndex, VklRay ray, float tMin, float tMax) {
        std::optional<HitRecord> result;

        VklBVH::BVHObject t = bvh_.objects[objectIndex];

        glm::vec3 n;
        glm::vec3 hit = triangleIntersection(ray, t.triangle, n);

        if (not (hit.y < 0 or hit.y > 1 or hit.z < 0 or hit.y + hit.z >1)) {
            if (hit.x > tMin and hit.x < tMax) {
                result.emplace();
                result->position = ray.at(hit.x);
                result->normal = glm::normalize(n);
                result->t = hit.x;
                result->materialIndex = t.object_index;
            }
        }

        return result;
    }

    std::optional<HitRecord> hitBvh(VklRay ray) {
        float tMin = 0.001;
        float tMax = 10000;

        bool hit_anything = false;
        std::optional<HitRecord> result;

        std::stack<int> nodeStack;
        nodeStack.push(0);

        float closest_so_far = tMax;

        while(not nodeStack.empty()) {
            int currentNode = nodeStack.top();
            nodeStack.pop();

            if (currentNode == -1) continue;
            int t = bvhTree.size();
            glm::vec2 tIntersect = intersectAABB(ray, bvhTree[currentNode].min, bvhTree[currentNode].max);
            if (tIntersect.x > tIntersect.y) continue;

            int index = bvhTree[currentNode].objectIndex;

            if (index != -1) {
                auto hit = hitTriangle(index, ray, tMin, closest_so_far);
                if (hit.has_value()) {
                    result = hit;
                    closest_so_far = hit->t;
                }
            }

            nodeStack.push(bvhTree[currentNode].leftNodeIndex);
            nodeStack.push(bvhTree[currentNode].rightNodeIndex);
        }

        return result;
    }

public:
    VklCpuRayTracer(VklScene &scene): scene_(scene), bvh_(scene) {}

    void performRayTracing() {
        bvhTree = bvh_.createGPUBVHTree();

        float h = std::tan(this->scene_.camera.zoom / 2);

        float viewportHeight = 2.0 * h;
        float viewportWidth = 2.0 * h;

        glm::vec3 vertical = viewportHeight * scene_.camera.camera_up_axis;
        glm::vec3 horizontal = viewportWidth * glm::normalize(glm::cross(scene_.camera.camera_front, scene_.camera.camera_up_axis));

        glm::vec3 lowerLeftCorner = scene_.camera.position + scene_.camera.camera_front - horizontal / 2.0f - vertical / 2.0f;

        for (int i = 0; i < n; i++) {
            std::cout << i << std::endl;
            for (int j = 0; j < m; j++) {
                // std::cout << j << std::endl;
                auto dir = lowerLeftCorner + vertical * (1.0f * i / n) + horizontal * (1.0f * j / m) - scene_.camera.position;
                auto res = hitBvh(VklRay(scene_.camera.position, dir));
                if (res.has_value()) {
                    output[i][j] = glm::vec4(1.0, 0, 0,1.0 );
                } else {
                    output[i][j] = glm::vec4(0, 0, 0, 1.0);
                }
            }
        }

        output_ppm_image(output);
    }
};