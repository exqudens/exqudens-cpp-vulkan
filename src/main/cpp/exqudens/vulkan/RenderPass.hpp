#pragma once

#include <optional>
#include <vector>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"
#include "exqudens/vulkan/SubpassDescription.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT RenderPass {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    std::vector<vk::AttachmentDescription> attachments;
    std::vector<SubpassDescription> subpasses;
    std::vector<vk::SubpassDependency> dependencies;
    vk::RenderPassCreateInfo createInfo;
    std::shared_ptr<vk::raii::RenderPass> value;

    vk::raii::RenderPass& reference();

  };

  class EXQUDENS_VULKAN_EXPORT RenderPass::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::AttachmentDescription> attachments;
      std::vector<SubpassDescription> subpasses;
      std::vector<vk::SubpassDependency> dependencies;
      std::optional<vk::RenderPassCreateInfo> createInfo;

    public:

      RenderPass::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      RenderPass::Builder& addAttachment(const vk::AttachmentDescription& val);

      RenderPass::Builder& setAttachments(const std::vector<vk::AttachmentDescription>& val);

      RenderPass::Builder& addSubpass(const SubpassDescription& val);

      RenderPass::Builder& setSubpasses(const std::vector<SubpassDescription>& val);

      RenderPass::Builder& addDependency(const vk::SubpassDependency& val);

      RenderPass::Builder& setDependencies(const std::vector<vk::SubpassDependency>& val);

      RenderPass::Builder& setCreateInfo(const vk::RenderPassCreateInfo& val);

      RenderPass build();

  };

}
