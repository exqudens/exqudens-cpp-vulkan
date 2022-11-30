#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT QueryPool {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    vk::QueryPoolCreateInfo createInfo;
    std::shared_ptr<vk::raii::QueryPool> value;

    vk::raii::QueryPool& reference();

  };

  class EXQUDENS_VULKAN_EXPORT QueryPool::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::QueryPoolCreateInfo> createInfo;

    public:

      QueryPool::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      QueryPool::Builder& setCreateInfo(const vk::QueryPoolCreateInfo& val);

      QueryPool build();

  };

}
