#pragma once

#include <optional>
#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/macros.hpp"

namespace exqudens::vulkan {

  struct CommandBuffer {

    class Builder;

    inline static Builder builder();

    vk::CommandBufferAllocateInfo createInfo;
    std::shared_ptr<vk::raii::CommandBuffer> value;

    vk::raii::CommandBuffer& reference() {
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

  class CommandBuffer::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::CommandBufferAllocateInfo> createInfo;

    public:

      CommandBuffer::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val) {
        device = val;
        return *this;
      }

      CommandBuffer::Builder& setCreateInfo(const vk::CommandBufferAllocateInfo& val) {
        createInfo = val;
        return *this;
      }

      CommandBuffer build() {
        try {
          CommandBuffer target = {};
          target.createInfo = createInfo.value();
          vk::raii::CommandBuffers values = vk::raii::CommandBuffers(
              *device.lock(),
              target.createInfo
          );
          target.value = std::make_shared<vk::raii::CommandBuffer>(std::move(values.front()));
          return target;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

  CommandBuffer::Builder CommandBuffer::builder() {
    return {};
  }

}
