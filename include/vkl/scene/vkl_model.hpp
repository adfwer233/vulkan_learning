#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "glm/glm.hpp"

#include "vkl/core/vkl_buffer.hpp"
#include "vkl/core/vkl_descriptor.hpp"
#include "vkl/core/vkl_device.hpp"
#include "vkl/core/vkl_texture.hpp"
#include "vkl/utils/vkl_box.hpp"

#include "vkl/templates/vkl_concept.hpp"

#include "geometry/mesh/mesh_model_template.hpp"
#include "geometry/renderable_geometry.hpp"

struct VklVertex2D: public Vertex2D {
    using geometry_type = Vertex2D;

    VklVertex2D() = default;
    VklVertex2D(Vertex2D vert): Vertex2D(vert) {};

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex2D);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back(
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex2D, position))});
        attributeDescriptions.push_back(
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex2D, color))});
        attributeDescriptions.push_back(
            {2, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex2D, normal))});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex2D, uv))});

        return attributeDescriptions;
    }
};

struct VklVertex3D: public Vertex3D {
    using geometry_type = Vertex3D;

    VklVertex3D() = default;
    VklVertex3D(Vertex3D vert): Vertex3D(vert) {};

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex3D);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back(
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex3D, position))});
        attributeDescriptions.push_back(
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex3D, color))});
        attributeDescriptions.push_back(
            {2, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex3D, normal))});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex3D, uv))});

        return attributeDescriptions;
    }
};

template <typename T>
concept VklBoxType = requires(T t) {
    t.min_position;
    t.max_position;
};

template <typename T>
concept VklIndexType = requires {
    {T::vertexCount} -> std::convertible_to<size_t>;
};

template <VklVertexType VertexType, VklIndexType IndexType = TriangleIndex, VklBoxType BoxType = VklBox3D>
class VklModelTemplate {

  public:
    typedef VertexType vertex_type;

    typedef IndexType index_type;

    struct BuilderFromImmediateData {
        std::vector<VertexType> vertices{};
        std::vector<IndexType> indices{};
        std::vector<std::string> texturePaths{};
    };

    std::vector<VklTexture *> textures_;

    std::vector<std::unique_ptr<VklBuffer>> uniformBuffers;
    std::vector<VkDescriptorSet> descriptorSets;

    std::unique_ptr<VklDescriptorSetLayout> descriptorSetLayout_;

    BoxType box; /** Axis aligned bounding box, has not transformed */

  private:
    VklDevice &device_;

    std::vector<std::unique_ptr<VklBuffer>> vertexBuffer_;
    std::vector<std::unique_ptr<VklBuffer>> indexBuffer_;

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
    void createIndexBuffers(const std::vector<IndexType> &indices);

    /**
     * @brief create texture image
     * @param texturePath
     */
    void createTextureImage(const std::string &texturePath);

  public:
    VklModelTemplate(VklDevice &device, BuilderFromImmediateData builder);

    VklModelTemplate(VklDevice &device, MeshModelTemplate<typename VertexType::geometry_type, IndexType> geometry);

    ~VklModelTemplate();

    VklModelTemplate(const VklModelTemplate &) = delete;
    VklModelTemplate &operator=(const VklModelTemplate &) = delete;

    std::unique_ptr<MeshModelTemplate<typename VertexType::geometry_type, IndexType>> geometry;

    META_GET_REGISTERED_TYPES(RenderableGeometryTag)::to<std::variant> underlayingGeometry;

    [[nodiscard]] int get_triangle_num() const;

    void createDescriptorSetLayout();

    void allocDescriptorSets(VklDescriptorSetLayout &setLayout, VklDescriptorPool &pool);

    void allocDescriptorSets(VklDescriptorPool &pool);

    void bind(VkCommandBuffer commandBuffer);

    void draw(VkCommandBuffer commandBuffer);

    void addTextureFromImage(VkImage image);

    int materialIndex = 0;

    VkBuffer getVertexBuffer(size_t index) {
        return vertexBuffer_[index]->getBuffer();
    };

    void updateVertexBuffer();

    friend class RayPicker;
    friend class VklBVH;
};

using VklModel = VklModelTemplate<VklVertex3D>;

#include "vkl_model.hpp.impl"