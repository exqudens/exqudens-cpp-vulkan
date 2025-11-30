#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT WriteDescriptorSet: vk::WriteDescriptorSet {

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& setDstSet(const vk::DescriptorSet& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& setDstBinding(const uint32_t& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& setDstArrayElement(const uint32_t& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& setDescriptorCount(const uint32_t& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& setDescriptorType(const vk::DescriptorType& val);

    std::vector<vk::DescriptorImageInfo> imageInfo;
    std::vector<vk::DescriptorBufferInfo> bufferInfo;
    std::vector<vk::BufferView> texelBufferView;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& addImageInfo(const vk::DescriptorImageInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& setImageInfo(const std::vector<vk::DescriptorImageInfo>& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& addBufferInfo(const vk::DescriptorBufferInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& setBufferInfo(const std::vector<vk::DescriptorBufferInfo>& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& addTexelBufferView(const vk::BufferView& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES WriteDescriptorSet& setTexelBufferView(const std::vector<vk::BufferView>& val);

  };

}
