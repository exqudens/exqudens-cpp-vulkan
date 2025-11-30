#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT DescriptorPool {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    std::vector<vk::DescriptorPoolSize> poolSizes;
    vk::DescriptorPoolCreateInfo createInfo;
    std::shared_ptr<vk::raii::DescriptorPool> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::DescriptorPool& reference();

  };

  class EXQUDENS_VULKAN_EXPORT DescriptorPool::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::DescriptorPoolSize> poolSizes;
      std::optional<vk::DescriptorPoolCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorPool::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorPool::Builder& addPoolSize(const vk::DescriptorPoolSize& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorPool::Builder& setPoolSizes(const std::vector<vk::DescriptorPoolSize>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorPool::Builder& setCreateInfo(const vk::DescriptorPoolCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorPool build();

  };

}
