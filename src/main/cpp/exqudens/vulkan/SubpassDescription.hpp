#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT SubpassDescription: vk::SubpassDescription {

    SubpassDescription& setFlags(const vk::SubpassDescriptionFlags& value);

    SubpassDescription& setPipelineBindPoint(const vk::PipelineBindPoint& value);

    std::vector<vk::AttachmentReference> inputAttachments;
    std::vector<vk::AttachmentReference> colorAttachments;
    std::vector<vk::AttachmentReference> resolveAttachments;
    std::optional<vk::AttachmentReference> depthStencilAttachment;
    std::vector<uint32_t> preserveAttachments;

    SubpassDescription& addInputAttachment(const vk::AttachmentReference& val);

    SubpassDescription& setInputAttachments(const std::vector<vk::AttachmentReference>& val);

    SubpassDescription& addColorAttachment(const vk::AttachmentReference& val);

    SubpassDescription& setColorAttachments(const std::vector<vk::AttachmentReference>& val);

    SubpassDescription& addResolveAttachment(const vk::AttachmentReference& val);

    SubpassDescription& setResolveAttachments(const std::vector<vk::AttachmentReference>& val);

    SubpassDescription& setDepthStencilAttachment(const vk::AttachmentReference& val);

    SubpassDescription& addPreserveAttachment(const uint32_t& val);

    SubpassDescription& setPreserveAttachments(const std::vector<uint32_t>& val);

  };

}
