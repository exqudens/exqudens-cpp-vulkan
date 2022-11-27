#pragma once

#include <optional>
#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/macros.hpp"

namespace exqudens::vulkan {

  struct Framebuffer {

    class Builder;

    inline static Builder builder();

    std::vector<vk::ImageView> attachments;
    vk::FramebufferCreateInfo createInfo;
    std::shared_ptr<vk::raii::Framebuffer> value;

    vk::raii::Framebuffer& reference() {
      try {
        if (!value) {
          throw std::runtime_error(CALL_INFO() + ": value is not initialized!");
        }
        return *value;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

  };

  class Framebuffer::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::ImageView> attachments;
      std::optional<vk::FramebufferCreateInfo> createInfo;

    public:

      Framebuffer::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val) {
        device = val;
        return *this;
      }

      Framebuffer::Builder& addAttachment(const vk::ImageView& val) {
        attachments.emplace_back(val);
        return *this;
      }

      Framebuffer::Builder& setAttachments(const std::vector<vk::ImageView>& val) {
        attachments = val;
        return *this;
      }

      Framebuffer::Builder& setCreateInfo(const vk::FramebufferCreateInfo& val) {
        createInfo = val;
        return *this;
      }

      Framebuffer build() {
        try {
          Framebuffer target = {};
          target.attachments = attachments;
          target.createInfo = createInfo.value();
          target.createInfo.setAttachments(target.attachments);
          target.value = std::make_shared<vk::raii::Framebuffer>(
              *device.lock(),
              target.createInfo
          );
          return target;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

  Framebuffer::Builder Framebuffer::builder() {
    return {};
  }

}
