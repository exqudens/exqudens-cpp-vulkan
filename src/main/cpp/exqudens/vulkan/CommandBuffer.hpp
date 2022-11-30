#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT CommandBuffer {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    vk::CommandBufferAllocateInfo createInfo;
    std::shared_ptr<vk::raii::CommandBuffer> value;

    vk::raii::CommandBuffer& reference();

  };

  class EXQUDENS_VULKAN_EXPORT CommandBuffer::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::CommandBufferAllocateInfo> createInfo;

    public:

      CommandBuffer::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      CommandBuffer::Builder& setCreateInfo(const vk::CommandBufferAllocateInfo& val);

      CommandBuffer build();

  };

}
