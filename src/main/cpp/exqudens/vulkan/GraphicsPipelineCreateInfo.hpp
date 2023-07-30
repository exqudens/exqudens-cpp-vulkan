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

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setFlags(const vk::PipelineCreateFlags& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setRenderPass(const vk::RenderPass& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setSubpass(const uint32_t& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setBasePipelineIndex(const int32_t& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setBasePipelineHandle(const vk::Pipeline& val);

    std::optional<PipelineVertexInputStateCreateInfo> vertexInputState;
    std::optional<vk::PipelineInputAssemblyStateCreateInfo> inputAssemblyState;
    std::optional<vk::PipelineTessellationStateCreateInfo> tessellationState;
    std::optional<PipelineViewportStateCreateInfo> viewportState;
    std::optional<vk::PipelineRasterizationStateCreateInfo> rasterizationState;
    std::optional<vk::PipelineMultisampleStateCreateInfo> multisampleState;
    std::optional<vk::PipelineDepthStencilStateCreateInfo> depthStencilState;
    std::optional<PipelineColorBlendStateCreateInfo> colorBlendState;
    std::optional<vk::PipelineDynamicStateCreateInfo> dynamicState;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setVertexInputState(const PipelineVertexInputStateCreateInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setInputAssemblyState(const vk::PipelineInputAssemblyStateCreateInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setTessellationState(const vk::PipelineTessellationStateCreateInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setViewportState(const PipelineViewportStateCreateInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setRasterizationState(const vk::PipelineRasterizationStateCreateInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setMultisampleState(const vk::PipelineMultisampleStateCreateInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setDepthStencilState(const vk::PipelineDepthStencilStateCreateInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setColorBlendState(const PipelineColorBlendStateCreateInfo& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES GraphicsPipelineCreateInfo& setDynamicState(const vk::PipelineDynamicStateCreateInfo& val);

  };

}
