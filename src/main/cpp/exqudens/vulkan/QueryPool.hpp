#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT QueryPool {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    vk::QueryPoolCreateInfo createInfo;
    std::shared_ptr<vk::raii::QueryPool> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::QueryPool& reference();

  };

  class EXQUDENS_VULKAN_EXPORT QueryPool::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::QueryPoolCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueryPool::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueryPool::Builder& setCreateInfo(const vk::QueryPoolCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueryPool build();

  };

}
