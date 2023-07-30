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

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    std::vector<vk::AttachmentDescription> attachments;
    std::vector<SubpassDescription> subpasses;
    std::vector<vk::SubpassDependency> dependencies;
    vk::RenderPassCreateInfo createInfo;
    std::shared_ptr<vk::raii::RenderPass> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::RenderPass& reference();

  };

  class EXQUDENS_VULKAN_EXPORT RenderPass::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::AttachmentDescription> attachments;
      std::vector<SubpassDescription> subpasses;
      std::vector<vk::SubpassDependency> dependencies;
      std::optional<vk::RenderPassCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES RenderPass::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES RenderPass::Builder& addAttachment(const vk::AttachmentDescription& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES RenderPass::Builder& setAttachments(const std::vector<vk::AttachmentDescription>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES RenderPass::Builder& addSubpass(const SubpassDescription& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES RenderPass::Builder& setSubpasses(const std::vector<SubpassDescription>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES RenderPass::Builder& addDependency(const vk::SubpassDependency& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES RenderPass::Builder& setDependencies(const std::vector<vk::SubpassDependency>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES RenderPass::Builder& setCreateInfo(const vk::RenderPassCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES RenderPass build();

  };

}
