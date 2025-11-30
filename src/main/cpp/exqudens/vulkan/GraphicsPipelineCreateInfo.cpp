#include "exqudens/vulkan/GraphicsPipelineCreateInfo.hpp"

namespace exqudens::vulkan {

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setFlags(const vk::PipelineCreateFlags& val) {
    vk::GraphicsPipelineCreateInfo::setFlags(val);
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setRenderPass(const vk::RenderPass& val) {
    vk::GraphicsPipelineCreateInfo::setRenderPass(val);
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setSubpass(const uint32_t& val) {
    vk::GraphicsPipelineCreateInfo::setSubpass(val);
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setBasePipelineIndex(const int32_t& val) {
    vk::GraphicsPipelineCreateInfo::setBasePipelineIndex(val);
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setBasePipelineHandle(const vk::Pipeline& val) {
    vk::GraphicsPipelineCreateInfo::setBasePipelineHandle(val);
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setVertexInputState(const exqudens::vulkan::PipelineVertexInputStateCreateInfo& val) {
    vertexInputState = val;
    vk::GraphicsPipelineCreateInfo::setPVertexInputState(&vertexInputState.value());
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setInputAssemblyState(const vk::PipelineInputAssemblyStateCreateInfo& val) {
    inputAssemblyState = val;
    vk::GraphicsPipelineCreateInfo::setPInputAssemblyState(&inputAssemblyState.value());
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setTessellationState(const vk::PipelineTessellationStateCreateInfo& val) {
    tessellationState = val;
    vk::GraphicsPipelineCreateInfo::setPTessellationState(&tessellationState.value());
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setViewportState(const exqudens::vulkan::PipelineViewportStateCreateInfo& val) {
    viewportState = val;
    vk::GraphicsPipelineCreateInfo::setPViewportState(&viewportState.value());
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setRasterizationState(const vk::PipelineRasterizationStateCreateInfo& val) {
    rasterizationState = val;
    vk::GraphicsPipelineCreateInfo::setPRasterizationState(&rasterizationState.value());
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setMultisampleState(const vk::PipelineMultisampleStateCreateInfo& val) {
    multisampleState = val;
    vk::GraphicsPipelineCreateInfo::setPMultisampleState(&multisampleState.value());
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setDepthStencilState(const vk::PipelineDepthStencilStateCreateInfo& val) {
    depthStencilState = val;
    vk::GraphicsPipelineCreateInfo::setPDepthStencilState(&depthStencilState.value());
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setColorBlendState(const exqudens::vulkan::PipelineColorBlendStateCreateInfo& val) {
    colorBlendState = val;
    vk::GraphicsPipelineCreateInfo::setPColorBlendState(&colorBlendState.value());
    return *this;
  }

  GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::setDynamicState(const vk::PipelineDynamicStateCreateInfo& val) {
    dynamicState = val;
    vk::GraphicsPipelineCreateInfo::setPDynamicState(&dynamicState.value());
    return *this;
  }

}
