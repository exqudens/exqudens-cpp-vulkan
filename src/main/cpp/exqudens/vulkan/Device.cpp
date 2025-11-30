#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Device.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::Device& Device::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Device::Builder& Device::Builder::setPhysicalDevice(const std::weak_ptr<vk::raii::PhysicalDevice>& val) {
    physicalDevice = val;
    return *this;
  }

  Device::Builder& Device::Builder::setCreateInfo(const vk::DeviceCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  Device Device::Builder::build() {
    try {
      Device target = {};
      target.createInfo = createInfo.value();
      target.value = std::make_shared<vk::raii::Device>(
          *physicalDevice.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Device::Builder Device::builder() {
    return {};
  }

}

#undef CALL_INFO
