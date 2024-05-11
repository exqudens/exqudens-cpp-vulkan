#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/CommandPool.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::CommandPool& CommandPool::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  CommandPool::Builder& CommandPool::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  CommandPool::Builder& CommandPool::Builder::setCreateInfo(const vk::CommandPoolCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  CommandPool CommandPool::Builder::build() {
    try {
      CommandPool target = {};
      target.createInfo = createInfo.value();
      target.value = std::make_shared<vk::raii::CommandPool>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  CommandPool::Builder CommandPool::builder() {
    return {};
  }

}

#undef CALL_INFO
