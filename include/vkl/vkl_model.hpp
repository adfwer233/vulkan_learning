#pragma once

#include <memory>
#include <string>

#include "glm/glm.hpp"

#include "vkl_buffer.hpp"
#include "vkl_device.hpp"
#include "vkl_texture.hpp"
#include "vkl_descriptor.hpp"

class VklModel {
  public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct FaceIndices {
        uint32_t i, j ,k;
    };

    struct BuilderFromFile {
        std::vector<Vertex> vertices{};
        std::vector<FaceIndices> indices{};
        void build(const std::string &filepath);
    };

    struct BuilderFromImmediateData {
        std::vector<Vertex> vertices{};
        std::vector<FaceIndices> indices{};
        std::vector<std::string> texturePaths{};
    };

    std::vector<VklTexture *> textures_;

    std::vector<std::unique_ptr<VklBuffer>> uniformBuffers;
    std::vector<VkDescriptorSet> descriptorSets;

  private:
    VklDevice &device_;

    std::vector<Vertex> vertices_{};
    std::vector<FaceIndices> indices_{};

    std::unique_ptr<VklBuffer> vertexBuffer_;
    std::unique_ptr<VklBuffer> indexBuffer_;

    uint32_t vertexCount_;
    uint32_t indexCount_;
    bool hasIndexBuffer = false;

    /**
     * @brief create vertex buffer (including staging buffer)
     *
     * staging buffer: belong to cpu, used to upload data
     * vertex buffer: only visible to gpu, used in real graphics pipeline
     *
     * Firstly, we create staging buffer, write data to it. Then create vertex buffer and copy data from the staging
     * buffer to the vertex buffer.
     *
     * @param vertices
     */
    void createVertexBuffers(const std::vector<Vertex> &vertices);

    /**
     * @brief create vertex buffer (including staging buffer)
     *
     * staging buffer: belong to cpu, used to upload data
     * vertex buffer: only visible to gpu, used in real graphics pipeline
     *
     * Firstly, we create staging buffer, write data to it. Then create vertex buffer and copy data from the staging
     * buffer to the vertex buffer.
     *
     * @param vertices
     */
    void createIndexBuffers(const std::vector<FaceIndices> &indices);

    /**
     * @brief create texture image
     * @param texturePath
     */
    void createTextureImage(const std::string &texturePath);

  public:
    VklModel(VklDevice &device, BuilderFromImmediateData builder);

    ~VklModel();

    VklModel(const VklModel &) = delete;
    VklModel &operator=(const VklModel &) = delete;

    [[nodiscard]] int get_triangle_num() const;

    void allocDescriptorSets(VklDescriptorSetLayout &setLayout, VklDescriptorPool &pool);

    void bind(VkCommandBuffer commandBuffer);

    void draw(VkCommandBuffer commandBuffer);

    friend class RayPicker;
};