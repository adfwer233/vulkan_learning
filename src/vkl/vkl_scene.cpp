#include "vkl/vkl_scene.hpp"
#include "vkl/vkl_object.hpp"

void VklScene::addObject(VklObject::ImportBuilder builder) {
    objects.push_back(std::make_unique<VklObject>(device_, builder));
    objects.back()->allocDescriptorSets(*setLayout_, *descriptorPool_);
}
