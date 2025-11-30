#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Framebuffer.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::Framebuffer& Framebuffer::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Framebuffer::Builder& Framebuffer::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  Framebuffer::Builder& Framebuffer::Builder::addAttachment(const vk::ImageView& val) {
    attachments.emplace_back(val);
    return *this;
  }

  Framebuffer::Builder& Framebuffer::Builder::setAttachments(const std::vector<vk::ImageView>& val) {
    attachments = val;
    return *this;
  }

  Framebuffer::Builder& Framebuffer::Builder::setCreateInfo(const vk::FramebufferCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  Framebuffer Framebuffer::Builder::build() {
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
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Framebuffer::Builder Framebuffer::builder() {
    return {};
  }

}

#undef CALL_INFO
