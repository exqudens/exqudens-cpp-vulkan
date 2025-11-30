#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT SubpassDescription: vk::SubpassDescription {

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& setFlags(const vk::SubpassDescriptionFlags& value);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& setPipelineBindPoint(const vk::PipelineBindPoint& value);

    std::vector<vk::AttachmentReference> inputAttachments;
    std::vector<vk::AttachmentReference> colorAttachments;
    std::vector<vk::AttachmentReference> resolveAttachments;
    std::optional<vk::AttachmentReference> depthStencilAttachment;
    std::vector<uint32_t> preserveAttachments;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& addInputAttachment(const vk::AttachmentReference& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& setInputAttachments(const std::vector<vk::AttachmentReference>& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& addColorAttachment(const vk::AttachmentReference& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& setColorAttachments(const std::vector<vk::AttachmentReference>& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& addResolveAttachment(const vk::AttachmentReference& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& setResolveAttachments(const std::vector<vk::AttachmentReference>& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& setDepthStencilAttachment(const vk::AttachmentReference& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& addPreserveAttachment(const uint32_t& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES SubpassDescription& setPreserveAttachments(const std::vector<uint32_t>& val);

  };

}
