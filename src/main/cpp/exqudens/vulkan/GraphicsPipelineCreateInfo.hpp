#pragma once

#include <cstdint>
#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"
#include "exqudens/vulkan/PipelineVertexInputStateCreateInfo.hpp"
#include "exqudens/vulkan/PipelineViewportStateCreateInfo.hpp"
#include "exqudens/vulkan/PipelineColorBlendStateCreateInfo.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT GraphicsPipelineCreateInfo: vk::GraphicsPipelineCreateInfo {

    GraphicsPipelineCreateInfo& setFlags(const vk::PipelineCreateFlags& val);

    GraphicsPipelineCreateInfo& setRenderPass(const vk::RenderPass& val);

    GraphicsPipelineCreateInfo& setSubpass(const uint32_t& val);

    GraphicsPipelineCreateInfo& setBasePipelineIndex(const int32_t& val);

    GraphicsPipelineCreateInfo& setBasePipelineHandle(const vk::Pipeline& val);

    std::optional<PipelineVertexInputStateCreateInfo> vertexInputState;
    std::optional<vk::PipelineInputAssemblyStateCreateInfo> inputAssemblyState;
    std::optional<vk::PipelineTessellationStateCreateInfo> tessellationState;
    std::optional<PipelineViewportStateCreateInfo> viewportState;
    std::optional<vk::PipelineRasterizationStateCreateInfo> rasterizationState;
    std::optional<vk::PipelineMultisampleStateCreateInfo> multisampleState;
    std::optional<vk::PipelineDepthStencilStateCreateInfo> depthStencilState;
    std::optional<PipelineColorBlendStateCreateInfo> colorBlendState;
    std::optional<vk::PipelineDynamicStateCreateInfo> dynamicState;

    GraphicsPipelineCreateInfo& setVertexInputState(const PipelineVertexInputStateCreateInfo& val);

    GraphicsPipelineCreateInfo& setInputAssemblyState(const vk::PipelineInputAssemblyStateCreateInfo& val);

    GraphicsPipelineCreateInfo& setTessellationState(const vk::PipelineTessellationStateCreateInfo& val);

    GraphicsPipelineCreateInfo& setViewportState(const PipelineViewportStateCreateInfo& val);

    GraphicsPipelineCreateInfo& setRasterizationState(const vk::PipelineRasterizationStateCreateInfo& val);

    GraphicsPipelineCreateInfo& setMultisampleState(const vk::PipelineMultisampleStateCreateInfo& val);

    GraphicsPipelineCreateInfo& setDepthStencilState(const vk::PipelineDepthStencilStateCreateInfo& val);

    GraphicsPipelineCreateInfo& setColorBlendState(const PipelineColorBlendStateCreateInfo& val);

    GraphicsPipelineCreateInfo& setDynamicState(const vk::PipelineDynamicStateCreateInfo& val);

  };

}
