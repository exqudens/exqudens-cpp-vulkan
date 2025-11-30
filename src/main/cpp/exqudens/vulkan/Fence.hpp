#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Fence {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    vk::FenceCreateInfo createInfo;
    std::shared_ptr<vk::raii::Fence> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Fence& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Fence::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::FenceCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Fence::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Fence::Builder& setCreateInfo(const vk::FenceCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Fence build();

  };

}
