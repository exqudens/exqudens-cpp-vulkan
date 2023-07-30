#pragma once

#include <array>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT PipelineColorBlendStateCreateInfo: vk::PipelineColorBlendStateCreateInfo {

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineColorBlendStateCreateInfo& setFlags(const vk::PipelineColorBlendStateCreateFlags& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineColorBlendStateCreateInfo& setLogicOpEnable(const bool& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineColorBlendStateCreateInfo& setLogicOp(const vk::LogicOp& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineColorBlendStateCreateInfo& setBlendConstants(const std::array<float, 4>& val);

    std::vector<vk::PipelineColorBlendAttachmentState> attachments;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineColorBlendStateCreateInfo& addAttachment(const vk::PipelineColorBlendAttachmentState& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineColorBlendStateCreateInfo& setAttachments(const std::vector<vk::PipelineColorBlendAttachmentState>& val);

  };

}
