#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT CommandPool {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    vk::CommandPoolCreateInfo createInfo;
    std::shared_ptr<vk::raii::CommandPool> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::CommandPool& reference();

  };

  class EXQUDENS_VULKAN_EXPORT CommandPool::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::CommandPoolCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES CommandPool::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES CommandPool::Builder& setCreateInfo(const vk::CommandPoolCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES CommandPool build();

  };

}
