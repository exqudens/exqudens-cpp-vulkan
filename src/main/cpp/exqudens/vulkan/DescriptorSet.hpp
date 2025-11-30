#pragma once

#include <optional>
#include <vector>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"
#include "exqudens/vulkan/WriteDescriptorSet.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT DescriptorSet {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    std::vector<vk::DescriptorSetLayout> setLayouts;
    vk::DescriptorSetAllocateInfo createInfo;
    std::vector<WriteDescriptorSet> writes;
    std::shared_ptr<vk::raii::DescriptorSet> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::DescriptorSet& reference();

  };

  class EXQUDENS_VULKAN_EXPORT DescriptorSet::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::DescriptorSetLayout> setLayouts;
      std::optional<vk::DescriptorSetAllocateInfo> createInfo;
      std::vector<WriteDescriptorSet> writes;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSet::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSet::Builder& addSetLayout(const vk::DescriptorSetLayout& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSet::Builder& setSetLayouts(const std::vector<vk::DescriptorSetLayout>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSet::Builder& setCreateInfo(const vk::DescriptorSetAllocateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSet::Builder& addWrite(const WriteDescriptorSet& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSet::Builder& setWrites(const std::vector<WriteDescriptorSet>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES DescriptorSet build();

  };

}
