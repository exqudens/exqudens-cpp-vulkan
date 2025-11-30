#include "exqudens/vulkan/PipelineColorBlendStateCreateInfo.hpp"

namespace exqudens::vulkan {

  PipelineColorBlendStateCreateInfo& PipelineColorBlendStateCreateInfo::setFlags(const vk::PipelineColorBlendStateCreateFlags& val) {
    vk::PipelineColorBlendStateCreateInfo::setFlags(val);
    return *this;
  }

  PipelineColorBlendStateCreateInfo& PipelineColorBlendStateCreateInfo::setLogicOpEnable(const bool& val) {
    vk::PipelineColorBlendStateCreateInfo::setLogicOpEnable(val);
    return *this;
  }

  PipelineColorBlendStateCreateInfo& PipelineColorBlendStateCreateInfo::setLogicOp(const vk::LogicOp& val) {
    vk::PipelineColorBlendStateCreateInfo::setLogicOp(val);
    return *this;
  }

  PipelineColorBlendStateCreateInfo& PipelineColorBlendStateCreateInfo::setBlendConstants(const std::array<float, 4>& val) {
    vk::PipelineColorBlendStateCreateInfo::setBlendConstants(val);
    return *this;
  }

  PipelineColorBlendStateCreateInfo& PipelineColorBlendStateCreateInfo::addAttachment(const vk::PipelineColorBlendAttachmentState& val) {
    attachments.emplace_back(val);
    setAttachments(attachments);
    return *this;
  }

  PipelineColorBlendStateCreateInfo& PipelineColorBlendStateCreateInfo::setAttachments(const std::vector<vk::PipelineColorBlendAttachmentState>& val) {
    attachments = val;
    vk::PipelineColorBlendStateCreateInfo::setAttachments(attachments);
    return *this;
  }

}
