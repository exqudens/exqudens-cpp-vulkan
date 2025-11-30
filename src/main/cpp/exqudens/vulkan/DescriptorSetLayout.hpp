#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT DescriptorSetLayout {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    vk::DescriptorSetLayoutCreateInfo createInfo;
    std::shared_ptr<vk::raii::DescriptorSetLayout> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::DescriptorSetLayout& reference();

  };

  class EXQUDENS_VULKAN_EXPORT DescriptorSetLayout::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::DescriptorSetLayoutBinding> bindings;
      std::optional<vk::DescriptorSetLayoutCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSetLayout::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSetLayout::Builder& addBinding(const vk::DescriptorSetLayoutBinding& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSetLayout::Builder& setBindings(const std::vector<vk::DescriptorSetLayoutBinding>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSetLayout::Builder& setCreateInfo(const vk::DescriptorSetLayoutCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSetLayout build();

  };

}
