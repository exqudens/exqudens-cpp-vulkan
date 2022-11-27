#pragma once

#include <optional>
#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/macros.hpp"

namespace exqudens::vulkan {

  struct CommandPool {

    class Builder;

    inline static Builder builder();

    vk::CommandPoolCreateInfo createInfo;
    std::shared_ptr<vk::raii::CommandPool> value;

    vk::raii::CommandPool& reference() {
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

  class CommandPool::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::CommandPoolCreateInfo> createInfo;

    public:

      CommandPool::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val) {
        device = val;
        return *this;
      }

      CommandPool::Builder& setCreateInfo(const vk::CommandPoolCreateInfo& val) {
        createInfo = val;
        return *this;
      }

      CommandPool build() {
        try {
          CommandPool target = {};
          target.createInfo = createInfo.value();
          target.value = std::make_shared<vk::raii::CommandPool>(
              *device.lock(),
              target.createInfo
          );
          return target;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

  CommandPool::Builder CommandPool::builder() {
    return {};
  }

}
