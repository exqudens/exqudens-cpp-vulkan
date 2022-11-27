#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT DescriptorSetLayout {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    vk::DescriptorSetLayoutCreateInfo createInfo;
    std::shared_ptr<vk::raii::DescriptorSetLayout> value;

    vk::raii::DescriptorSetLayout& reference();

  };

  class EXQUDENS_VULKAN_EXPORT DescriptorSetLayout::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::DescriptorSetLayoutBinding> bindings;
      std::optional<vk::DescriptorSetLayoutCreateInfo> createInfo;

    public:

      DescriptorSetLayout::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      DescriptorSetLayout::Builder& addBinding(const vk::DescriptorSetLayoutBinding& val);

      DescriptorSetLayout::Builder& setBindings(const std::vector<vk::DescriptorSetLayoutBinding>& val);

      DescriptorSetLayout::Builder& setCreateInfo(const vk::DescriptorSetLayoutCreateInfo& val);

      DescriptorSetLayout build();

  };

}
