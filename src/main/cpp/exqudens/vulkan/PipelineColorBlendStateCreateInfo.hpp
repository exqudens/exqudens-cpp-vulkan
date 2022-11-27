#pragma once

#include <array>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT PipelineColorBlendStateCreateInfo: vk::PipelineColorBlendStateCreateInfo {

    PipelineColorBlendStateCreateInfo& setFlags(const vk::PipelineColorBlendStateCreateFlags& val);

    PipelineColorBlendStateCreateInfo& setLogicOpEnable(const bool& val);

    PipelineColorBlendStateCreateInfo& setLogicOp(const vk::LogicOp& val);

    PipelineColorBlendStateCreateInfo& setBlendConstants(const std::array<float, 4>& val);

    std::vector<vk::PipelineColorBlendAttachmentState> attachments;

    PipelineColorBlendStateCreateInfo& addAttachment(const vk::PipelineColorBlendAttachmentState& val);

    PipelineColorBlendStateCreateInfo& setAttachments(const std::vector<vk::PipelineColorBlendAttachmentState>& val);

  };

}
