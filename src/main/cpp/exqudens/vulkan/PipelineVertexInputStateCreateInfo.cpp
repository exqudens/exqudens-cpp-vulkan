#include "exqudens/vulkan/PipelineVertexInputStateCreateInfo.hpp"

namespace exqudens::vulkan {

  PipelineVertexInputStateCreateInfo& PipelineVertexInputStateCreateInfo::addVertexBindingDescription(const vk::VertexInputBindingDescription& val) {
    vertexBindingDescriptions.emplace_back(val);
    setVertexBindingDescriptions(vertexBindingDescriptions);
    return *this;
  }

  PipelineVertexInputStateCreateInfo& PipelineVertexInputStateCreateInfo::setVertexBindingDescriptions(const std::vector<vk::VertexInputBindingDescription>& val) {
    vertexBindingDescriptions = val;
    vk::PipelineVertexInputStateCreateInfo::setVertexBindingDescriptions(vertexBindingDescriptions);
    return *this;
  }

  PipelineVertexInputStateCreateInfo& PipelineVertexInputStateCreateInfo::addVertexAttributeDescription(const vk::VertexInputAttributeDescription& val) {
    vertexAttributeDescriptions.emplace_back(val);
    setVertexAttributeDescriptions(vertexAttributeDescriptions);
    return *this;
  }

  PipelineVertexInputStateCreateInfo& PipelineVertexInputStateCreateInfo::setVertexAttributeDescriptions(const std::vector<vk::VertexInputAttributeDescription>& val) {
    vertexAttributeDescriptions = val;
    vk::PipelineVertexInputStateCreateInfo::setVertexAttributeDescriptions(vertexAttributeDescriptions);
    return *this;
  }

}
