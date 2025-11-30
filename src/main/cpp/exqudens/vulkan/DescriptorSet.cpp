#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/DescriptorSet.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::DescriptorSet& DescriptorSet::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  DescriptorSet::Builder& DescriptorSet::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  DescriptorSet::Builder& DescriptorSet::Builder::addSetLayout(const vk::DescriptorSetLayout& val) {
    setLayouts.emplace_back(val);
    return *this;
  }

  DescriptorSet::Builder& DescriptorSet::Builder::setSetLayouts(const std::vector<vk::DescriptorSetLayout>& val) {
    setLayouts = val;
    return *this;
  }

  DescriptorSet::Builder& DescriptorSet::Builder::setCreateInfo(const vk::DescriptorSetAllocateInfo& val) {
    createInfo = val;
    return *this;
  }

  DescriptorSet::Builder& DescriptorSet::Builder::addWrite(const exqudens::vulkan::WriteDescriptorSet& val) {
    writes.emplace_back(val);
    return *this;
  }

  DescriptorSet::Builder& DescriptorSet::Builder::setWrites(const std::vector<WriteDescriptorSet>& val) {
    writes = val;
    return *this;
  }

  DescriptorSet DescriptorSet::Builder::build() {
    try {
      DescriptorSet target = {};
      target.setLayouts = setLayouts;
      target.createInfo = createInfo.value();
      target.createInfo.setSetLayouts(target.setLayouts);
      vk::raii::DescriptorSets values = vk::raii::DescriptorSets(
          *device.lock(),
          target.createInfo
      );
      target.value = std::make_shared<vk::raii::DescriptorSet>(std::move(values.front()));
      target.writes = writes;
      std::vector<vk::WriteDescriptorSet> tmpWrites;
      for (WriteDescriptorSet& write : target.writes) {
        write.setDstSet(*target.reference());
        tmpWrites.emplace_back(write);
      }
      device.lock()->updateDescriptorSets(tmpWrites, {});
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  DescriptorSet::Builder DescriptorSet::builder() {
    return {};
  }

}

#undef CALL_INFO
