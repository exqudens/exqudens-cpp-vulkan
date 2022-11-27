#include "exqudens/vulkan/SubpassDescription.hpp"

namespace exqudens::vulkan {

  SubpassDescription& SubpassDescription::setFlags(const vk::SubpassDescriptionFlags& value) {
    vk::SubpassDescription::setFlags(value);
    return *this;
  }

  SubpassDescription& SubpassDescription::setPipelineBindPoint(const vk::PipelineBindPoint& value) {
    vk::SubpassDescription::setPipelineBindPoint(value);
    return *this;
  }

  SubpassDescription& SubpassDescription::addInputAttachment(const vk::AttachmentReference& val) {
    inputAttachments.emplace_back(val);
    vk::SubpassDescription::setInputAttachments(inputAttachments);
    return *this;
  }

  SubpassDescription& SubpassDescription::setInputAttachments(const std::vector<vk::AttachmentReference>& val) {
    inputAttachments = val;
    vk::SubpassDescription::setInputAttachments(inputAttachments);
    return *this;
  }

  SubpassDescription& SubpassDescription::addColorAttachment(const vk::AttachmentReference& val) {
    colorAttachments.emplace_back(val);
    vk::SubpassDescription::setColorAttachments(colorAttachments);
    return *this;
  }

  SubpassDescription& SubpassDescription::setColorAttachments(const std::vector<vk::AttachmentReference>& val) {
    colorAttachments = val;
    vk::SubpassDescription::setColorAttachments(colorAttachments);
    return *this;
  }

  SubpassDescription& SubpassDescription::addResolveAttachment(const vk::AttachmentReference& val) {
    resolveAttachments.emplace_back(val);
    vk::SubpassDescription::setResolveAttachments(resolveAttachments);
    return *this;
  }

  SubpassDescription& SubpassDescription::setResolveAttachments(const std::vector<vk::AttachmentReference>& val) {
    resolveAttachments = val;
    vk::SubpassDescription::setResolveAttachments(resolveAttachments);
    return *this;
  }

  SubpassDescription& SubpassDescription::setDepthStencilAttachment(const vk::AttachmentReference& val) {
    depthStencilAttachment = val;
    vk::SubpassDescription::setPDepthStencilAttachment(&depthStencilAttachment.value());
    return *this;
  }

  SubpassDescription& SubpassDescription::addPreserveAttachment(const uint32_t& val) {
    preserveAttachments.emplace_back(val);
    vk::SubpassDescription::setPreserveAttachments(preserveAttachments);
    return *this;
  }

  SubpassDescription& SubpassDescription::setPreserveAttachments(const std::vector<uint32_t>& val) {
    preserveAttachments = val;
    vk::SubpassDescription::setPreserveAttachments(preserveAttachments);
    return *this;
  }

}
