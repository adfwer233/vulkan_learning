#include "vkl/scene/vkl_scene.hpp"

void VklScene::addObject(VklModel::BuilderFromImmediateData builder) {
    objects.push_back(std::make_unique<VklObject>(device_, builder));
    objects.back()->allocDescriptorSets(*setLayout_, *descriptorPool_);
}
