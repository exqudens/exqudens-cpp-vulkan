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

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    std::vector<vk::DescriptorSetLayout> setLayouts;
    vk::DescriptorSetAllocateInfo createInfo;
    std::vector<WriteDescriptorSet> writes;
    std::shared_ptr<vk::raii::DescriptorSet> value;

    vk::raii::DescriptorSet& reference();

  };

  class EXQUDENS_VULKAN_EXPORT DescriptorSet::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::DescriptorSetLayout> setLayouts;
      std::optional<vk::DescriptorSetAllocateInfo> createInfo;
      std::vector<WriteDescriptorSet> writes;

    public:

      DescriptorSet::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      DescriptorSet::Builder& addSetLayout(const vk::DescriptorSetLayout& val);

      DescriptorSet::Builder& setSetLayouts(const std::vector<vk::DescriptorSetLayout>& val);

      DescriptorSet::Builder& setCreateInfo(const vk::DescriptorSetAllocateInfo& val);

      DescriptorSet::Builder& addWrite(const WriteDescriptorSet& val);

      DescriptorSet::Builder& setWrites(const std::vector<WriteDescriptorSet>& val);

      DescriptorSet build();

  };

}
