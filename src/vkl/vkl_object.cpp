#include "vkl_object.hpp"

#include "io/assimp_loader.hpp"

VklObject::VklObject(VklDevice &device, VklObject::ImportBuilder builder) : device_(device) {
    AssimpLoader assimpLoader;
    auto modelBuilders = assimpLoader.read_model(builder.modelPath);

    for (auto modelBuilder : modelBuilders) {
        this->models.push_back(new VklModel(device, modelBuilder));
    }

}

VklObject::~VklObject() {
    for (auto model : models) {
        delete model;
    }
}

void VklObject::render_object() {
}

void VklObject::allocDescriptorSets(VklDescriptorSetLayout &setLayout, VklDescriptorPool &pool) {
    for (auto model: models) {
        model->allocDescriptorSets(setLayout, pool);
    }
}

int VklObject::get_triangle_num() {
    int result = 0;
    for (auto model: models) {
        result += model->get_triangle_num();
    }
    return result;
}
