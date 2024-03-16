#include "vkl/bvh/vkl_bvh.hpp"

#include <algorithm>
#include <stack>
#include <ranges>

const glm::vec3 eps(0.0001);

std::vector<VklBVHGPUModel::BVHNode> VklBVH::createGPUBVHTree() {
    // create bvh objects
    this->objects.clear();

    for (size_t i = 0; i < scene_.objects.size(); i++) {
        auto &object = scene_.objects[i];
        for (size_t j = 0; j < object->models.size(); j++) {
            auto model = object->models[i];

            for (size_t k = 0; k < model->indices_.size(); i++) {
                auto tri_indices = model->indices_[k];
                BVHObject bvhObject;
                bvhObject.object_index = objects.size();
                bvhObject.triangle = VklBVHGPUModel::Triangle{model->vertices_[tri_indices.i].position, model->vertices_[tri_indices.j].position, model->vertices_[tri_indices.k].position, 0};

                objects.push_back(bvhObject);
            }
        }
    }

    std::vector<BVHNode> intermediate;
    int nodeCounter = 0;
    std::stack<BVHNode> nodeStack;

    BVHNode root;
    root.index = nodeCounter;
    root.objects = objects;
    nodeCounter++;

    while(!nodeStack.empty()) {
        BVHNode currentNode = nodeStack.top();
        nodeStack.pop();

        currentNode.box = objectListBoundingBox(currentNode.objects);

        int axis = rand() % 3;

        auto comparator = (axis == 0)   ? boxXCompare
                                        : (axis == 1) ? boxYCompare
                                                      : boxZCompare;

        std::ranges::sort(currentNode.objects, comparator);

        int n = currentNode.objects.size();
        if (n <= 1) {
            intermediate.push_back(currentNode);
            continue;
        } else {
            auto mid = n >> 1;

            BVHNode leftNode, rightNode;

            leftNode.index = nodeCounter;
            nodeCounter++;
            std::copy(currentNode.objects.begin(), currentNode.objects.begin() + mid, std::back_inserter(leftNode.objects));
            nodeStack.push(leftNode);

            rightNode.index = nodeCounter;
            nodeCounter++;
            std::copy(currentNode.objects.begin() + mid, currentNode.objects.end(), std::back_inserter(rightNode.objects));
            nodeStack.push(rightNode);

            currentNode.leftNodeIndex = leftNode.index;
            currentNode.rightNodeIndex = rightNode.index;

            intermediate.push_back(currentNode);
        }
    }

    std::ranges::sort(intermediate, [](BVHNode &a, BVHNode &b){return a.index < b.index;});

    std::vector<VklBVHGPUModel::BVHNode> output;
    output.reserve(intermediate.size());
    for (auto& node: intermediate) {
        VklBVHGPUModel::BVHNode gpuNode;
        gpuNode.leftNodeIndex = node.leftNodeIndex;
        gpuNode.rightNodeIndex = node.rightNodeIndex;
        gpuNode.min = node.box.min;
        gpuNode.max = node.box.max;
        if (node.leftNodeIndex == -1 || node.rightNodeIndex == -1) {
            gpuNode.objectIndex = node.objects[0].object_index;
        } else {
            gpuNode.objectIndex = -1;
        }
    }

    return output;
}

VklBVH::AABB VklBVH::objectBoundingBox(VklBVHGPUModel::Triangle &tri) {
    return {glm::min(glm::min(tri.v0, tri.v1), tri.v2) - eps, glm::max(glm::max(tri.v0, tri.v1), tri.v2) + eps};
}

VklBVH::AABB VklBVH::surroundingBox(VklBVH::AABB box0, VklBVH::AABB box1) {
    return {glm::min(box0.min, box1.min), glm::max(box0.max, box1.max)};
}

VklBVH::AABB VklBVH::objectListBoundingBox(std::vector<BVHObject> &objects) {
    AABB tempBox;
    AABB outputBox;
    bool firstBox = true;

    for (auto &object : objects)
    {
        tempBox = objectBoundingBox(object.triangle);
        outputBox = firstBox ? tempBox : surroundingBox(outputBox, tempBox);
        firstBox = false;
    }

    return outputBox;
}

