#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT WriteDescriptorSet: vk::WriteDescriptorSet {

    WriteDescriptorSet& setDstSet(const vk::DescriptorSet& val);

    WriteDescriptorSet& setDstBinding(const uint32_t& val);

    WriteDescriptorSet& setDstArrayElement(const uint32_t& val);

    WriteDescriptorSet& setDescriptorCount(const uint32_t& val);

    WriteDescriptorSet& setDescriptorType(const vk::DescriptorType& val);

    std::vector<vk::DescriptorImageInfo> imageInfo;
    std::vector<vk::DescriptorBufferInfo> bufferInfo;
    std::vector<vk::BufferView> texelBufferView;

    WriteDescriptorSet& addImageInfo(const vk::DescriptorImageInfo& val);

    WriteDescriptorSet& setImageInfo(const std::vector<vk::DescriptorImageInfo>& val);

    WriteDescriptorSet& addBufferInfo(const vk::DescriptorBufferInfo& val);

    WriteDescriptorSet& setBufferInfo(const std::vector<vk::DescriptorBufferInfo>& val);

    WriteDescriptorSet& addTexelBufferView(const vk::BufferView& val);

    WriteDescriptorSet& setTexelBufferView(const std::vector<vk::BufferView>& val);

  };

}
