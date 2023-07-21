#pragma once

#include <optional>
#include <vector>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Framebuffer {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    std::vector<vk::ImageView> attachments;
    vk::FramebufferCreateInfo createInfo;
    std::shared_ptr<vk::raii::Framebuffer> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Framebuffer& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Framebuffer::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::ImageView> attachments;
      std::optional<vk::FramebufferCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Framebuffer::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Framebuffer::Builder& addAttachment(const vk::ImageView& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Framebuffer::Builder& setAttachments(const std::vector<vk::ImageView>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Framebuffer::Builder& setCreateInfo(const vk::FramebufferCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Framebuffer build();

  };

}
