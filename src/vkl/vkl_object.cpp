#include "vkl_object.hpp"

#include "io/assimp_loader.hpp"

VklObject::VklObject(VklDevice &device, VklObject::ImportBuilder builder): device_(device) {
    AssimpLoader assimpLoader;
    auto modelBuilders = assimpLoader.read_model(builder.modelPath);

    for (auto modelBuilder: modelBuilders) {
        this->models.push_back(new VklModel(device, modelBuilder));
    }
}

VklObject::~VklObject() {
    for (auto model: models) {
        delete model;
    }
}

void VklObject::render_object() {

}
