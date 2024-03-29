#include "vkl/scene/vkl_object.hpp"

#include "glm/gtc/quaternion.hpp"
#include "vkl/io/assimp_loader.hpp"

VklObject::~VklObject() {
    for (auto model : models) {
        delete model;
    }
}

void VklObject::render_object() {
}

void VklObject::allocDescriptorSets(VklDescriptorSetLayout &setLayout, VklDescriptorPool &pool) {
    for (auto model : models) {
        model->allocDescriptorSets(setLayout, pool);
    }
}

int VklObject::get_triangle_num() {
    int result = 0;
    for (auto model : models) {
        result += model->get_triangle_num();
    }
    return result;
}

glm::mat4 VklObject::getModelTransformation() {
    glm::mat4 model(1.0f);
    model = glm::translate(model, modelTranslation);
    model = glm::rotate(model, modelRotation.w, glm::axis(modelRotation));
    model = glm::scale(model, modelScaling);

    return model;
}

void VklObject::allocDescriptorSets(VklDescriptorPool &pool) {
    for (auto model : models) {
        model->allocDescriptorSets(pool);
    }
}
