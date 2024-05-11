#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Queue.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::Queue& Queue::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Queue::Builder& Queue::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  Queue::Builder& Queue::Builder::setFamilyIndex(const uint32_t& val) {
    familyIndex = val;
    return *this;
  }

  Queue::Builder& Queue::Builder::setIndex(const uint32_t& val) {
    index = val;
    return *this;
  }

  Queue Queue::Builder::build() {
    try {
      Queue target = {};
      target.familyIndex = familyIndex.value();
      target.index = index.value_or(0);
      target.value = std::make_shared<vk::raii::Queue>(
          *device.lock(),
          target.familyIndex,
          target.index
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Queue::Builder Queue::builder() {
    return {};
  }

}

#undef CALL_INFO
