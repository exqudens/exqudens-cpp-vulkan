#pragma once

#include <optional>
#include <vector>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Framebuffer {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    std::vector<vk::ImageView> attachments;
    vk::FramebufferCreateInfo createInfo;
    std::shared_ptr<vk::raii::Framebuffer> value;

    vk::raii::Framebuffer& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Framebuffer::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::ImageView> attachments;
      std::optional<vk::FramebufferCreateInfo> createInfo;

    public:

      Framebuffer::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      Framebuffer::Builder& addAttachment(const vk::ImageView& val);

      Framebuffer::Builder& setAttachments(const std::vector<vk::ImageView>& val);

      Framebuffer::Builder& setCreateInfo(const vk::FramebufferCreateInfo& val);

      Framebuffer build();

  };

}
