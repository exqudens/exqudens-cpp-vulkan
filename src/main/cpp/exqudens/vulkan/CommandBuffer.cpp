#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/CommandBuffer.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::CommandBuffer& CommandBuffer::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  CommandBuffer::Builder& CommandBuffer::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  CommandBuffer::Builder& CommandBuffer::Builder::setCreateInfo(const vk::CommandBufferAllocateInfo& val) {
    createInfo = val;
    return *this;
  }

  CommandBuffer CommandBuffer::Builder::build() {
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
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  CommandBuffer::Builder CommandBuffer::builder() {
    return {};
  }

}

#undef CALL_INFO
