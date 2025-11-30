#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT PipelineVertexInputStateCreateInfo: vk::PipelineVertexInputStateCreateInfo {

    std::vector<vk::VertexInputBindingDescription> vertexBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineVertexInputStateCreateInfo& addVertexBindingDescription(const vk::VertexInputBindingDescription& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineVertexInputStateCreateInfo& setVertexBindingDescriptions(const std::vector<vk::VertexInputBindingDescription>& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineVertexInputStateCreateInfo& addVertexAttributeDescription(const vk::VertexInputAttributeDescription& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineVertexInputStateCreateInfo& setVertexAttributeDescriptions(const std::vector<vk::VertexInputAttributeDescription>& val);

  };

}
