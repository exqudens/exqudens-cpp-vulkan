#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Fence.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::Fence& Fence::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Fence::Builder& Fence::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  Fence::Builder& Fence::Builder::setCreateInfo(const vk::FenceCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  Fence Fence::Builder::build() {
    try {
      Fence target = {};
      target.createInfo = createInfo.value_or(vk::FenceCreateInfo());
      target.value = std::make_shared<vk::raii::Fence>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Fence::Builder Fence::builder() {
    return {};
  }

}

#undef CALL_INFO
