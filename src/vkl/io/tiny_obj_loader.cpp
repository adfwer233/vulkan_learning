#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "vkl/io/tiny_obj_loader.hpp"

std::vector<MeshModel> VklTinyObjLoader::load_model()  {
    std::vector<MeshModel> result;

    tinyobj::ObjReaderConfig readerConfig;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path_, readerConfig)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        auto &model = result.emplace_back();

        for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
            decltype(model.vertices)::value_type vertex;

            tinyobj::real_t vx = attrib.vertices[3*size_t(v)+0];
            tinyobj::real_t vy = attrib.vertices[3*size_t(v)+1];
            tinyobj::real_t vz = attrib.vertices[3*size_t(v)+2];

            vertex.position = {vx, -vy, vz};

            if (not attrib.normals.empty()) {
                tinyobj::real_t nx = attrib.normals[3*size_t(v)+0];
                tinyobj::real_t ny = attrib.normals[3*size_t(v)+1];
                tinyobj::real_t nz = attrib.normals[3*size_t(v)+2];

                vertex.normal = {nx, -ny, nz};
            }

            if (not attrib.texcoords.empty()) {
                tinyobj::real_t tx = attrib.texcoords[2 * size_t(v) + 0];
                tinyobj::real_t ty = attrib.texcoords[2 * size_t(v) + 1];

                vertex.uv = {tx, ty};
            }

            model.vertices.push_back(vertex);
        }

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            if (fv != 3)
                throw std::runtime_error(std::format("not triangular mesh in {}", __FUNCTION__));

            tinyobj::index_t idx1 = shapes[s].mesh.indices[index_offset];
            tinyobj::index_t idx2 = shapes[s].mesh.indices[index_offset + 1];
            tinyobj::index_t idx3 = shapes[s].mesh.indices[index_offset + 2];

            model.indices.push_back({static_cast<uint32_t>(idx1.vertex_index), static_cast<uint32_t>(idx2.vertex_index), static_cast<uint32_t>(idx3.vertex_index)});
            index_offset += fv;
        }
    }

    std::cout << "tiny loaded" << std::endl;

    return result;
}