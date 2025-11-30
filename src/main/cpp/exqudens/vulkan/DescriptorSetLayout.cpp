#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/DescriptorSetLayout.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::DescriptorSetLayout& DescriptorSetLayout::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding(const vk::DescriptorSetLayoutBinding& val) {
    bindings.emplace_back(val);
    return *this;
  }

  DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::setBindings(const std::vector<vk::DescriptorSetLayoutBinding>& val) {
    bindings = val;
    return *this;
  }

  DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::setCreateInfo(const vk::DescriptorSetLayoutCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  DescriptorSetLayout DescriptorSetLayout::Builder::build() {
    try {
      DescriptorSetLayout target = {};
      target.bindings = bindings;
      target.createInfo = createInfo.value_or(vk::DescriptorSetLayoutCreateInfo());
      target.createInfo.setBindings(target.bindings);
      target.value = std::make_shared<vk::raii::DescriptorSetLayout>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  DescriptorSetLayout::Builder DescriptorSetLayout::builder() {
    return {};
  }

}

#undef CALL_INFO
