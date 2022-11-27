#pragma once

#include <optional>
#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/macros.hpp"

namespace exqudens::vulkan {

  struct Queue {

    class Builder;

    inline static Builder builder();

    uint32_t familyIndex;
    uint32_t index;
    std::shared_ptr<vk::raii::Queue> value;

    vk::raii::Queue& reference() {
      try {
        if (!value) {
          throw std::runtime_error(CALL_INFO() + ": value is not initialized!");
        }
        return *value;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

  };

  class Queue::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<uint32_t> familyIndex;
      std::optional<uint32_t> index;

    public:

      Queue::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val) {
        device = val;
        return *this;
      }

      Queue::Builder& setFamilyIndex(const uint32_t& val) {
        familyIndex = val;
        return *this;
      }

      Queue::Builder& setIndex(const uint32_t& val) {
        index = val;
        return *this;
      }

      Queue build() {
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
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

  Queue::Builder Queue::builder() {
    return {};
  }

}
