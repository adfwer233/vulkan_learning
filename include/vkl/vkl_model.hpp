#pragma once

#include <memory>
#include <string>

#include "glm/glm.hpp"

#include "vkl_buffer.hpp"
#include "vkl_descriptor.hpp"
#include "vkl_device.hpp"
#include "vkl_texture.hpp"

#include "templates/vkl_concept.hpp"

struct Vertex3D {
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex3D);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(){
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back(
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex3D, position))});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex3D, color))});
        attributeDescriptions.push_back(
                {2, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex3D, normal))});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex3D, uv))});

        return attributeDescriptions;
    }
};

template<typename VertexType>
class VklModelTemplate {

  public:
    typedef VertexType vertex_type;

    struct FaceIndices {
        uint32_t i, j, k;
    };

    struct BuilderFromFile {
        std::vector<VertexType> vertices{};
        std::vector<FaceIndices> indices{};
        void build(const std::string &filepath);
    };

    struct BuilderFromImmediateData {
        std::vector<VertexType> vertices{};
        std::vector<FaceIndices> indices{};
        std::vector<std::string> texturePaths{};
    };

    std::vector<VklTexture *> textures_;

    std::vector<std::unique_ptr<VklBuffer>> uniformBuffers;
    std::vector<VkDescriptorSet> descriptorSets;

  private:
    VklDevice &device_;

    std::vector<VertexType> vertices_{};
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
    void createVertexBuffers(const std::vector<VertexType> &vertices);

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
    VklModelTemplate(VklDevice &device, BuilderFromImmediateData builder);

    ~VklModelTemplate();

    VklModelTemplate(const VklModelTemplate &) = delete;
    VklModelTemplate &operator=(const VklModelTemplate &) = delete;

    [[nodiscard]] int get_triangle_num() const;

    void allocDescriptorSets(VklDescriptorSetLayout &setLayout, VklDescriptorPool &pool);

    void bind(VkCommandBuffer commandBuffer);

    void draw(VkCommandBuffer commandBuffer);

    friend class RayPicker;
};

using VklModel = VklModelTemplate<Vertex3D>;

#include "templates/vkl_model.hpp.impl"