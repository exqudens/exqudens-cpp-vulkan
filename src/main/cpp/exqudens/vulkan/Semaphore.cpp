#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Semaphore.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::Semaphore& Semaphore::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Semaphore::Builder& Semaphore::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  Semaphore::Builder& Semaphore::Builder::setCreateInfo(const vk::SemaphoreCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  Semaphore Semaphore::Builder::build() {
    try {
      Semaphore target = {};
      target.createInfo = createInfo.value_or(vk::SemaphoreCreateInfo());
      target.value = std::make_shared<vk::raii::Semaphore>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Semaphore::Builder Semaphore::builder() {
    return {};
  }

}

#undef CALL_INFO
