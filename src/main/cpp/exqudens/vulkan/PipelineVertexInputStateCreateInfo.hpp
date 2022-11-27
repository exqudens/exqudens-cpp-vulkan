#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT PipelineVertexInputStateCreateInfo: vk::PipelineVertexInputStateCreateInfo {

    std::vector<vk::VertexInputBindingDescription> vertexBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions;

    PipelineVertexInputStateCreateInfo& addVertexBindingDescription(const vk::VertexInputBindingDescription& val);

    PipelineVertexInputStateCreateInfo& setVertexBindingDescriptions(const std::vector<vk::VertexInputBindingDescription>& val);

    PipelineVertexInputStateCreateInfo& addVertexAttributeDescription(const vk::VertexInputAttributeDescription& val);

    PipelineVertexInputStateCreateInfo& setVertexAttributeDescriptions(const std::vector<vk::VertexInputAttributeDescription>& val);

  };

}
