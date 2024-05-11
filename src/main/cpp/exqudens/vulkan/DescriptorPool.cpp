#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/DescriptorPool.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::DescriptorPool& DescriptorPool::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  DescriptorPool::Builder& DescriptorPool::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(const vk::DescriptorPoolSize& val) {
    poolSizes.emplace_back(val);
    return *this;
  }

  DescriptorPool::Builder& DescriptorPool::Builder::setPoolSizes(const std::vector<vk::DescriptorPoolSize>& val) {
    poolSizes = val;
    return *this;
  }

  DescriptorPool::Builder& DescriptorPool::Builder::setCreateInfo(const vk::DescriptorPoolCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  DescriptorPool DescriptorPool::Builder::build() {
    try {
      DescriptorPool target = {};
      target.poolSizes = poolSizes;
      target.createInfo = createInfo.value();
      target.createInfo.setPoolSizes(target.poolSizes);
      //target.createInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
      target.value = std::make_shared<vk::raii::DescriptorPool>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  DescriptorPool::Builder DescriptorPool::builder() {
    return {};
  }

}

#undef CALL_INFO
