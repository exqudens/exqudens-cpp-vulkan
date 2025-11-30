#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/RenderPass.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::RenderPass& RenderPass::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  RenderPass::Builder& RenderPass::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  RenderPass::Builder& RenderPass::Builder::addAttachment(const vk::AttachmentDescription& val) {
    attachments.emplace_back(val);
    return *this;
  }

  RenderPass::Builder& RenderPass::Builder::setAttachments(const std::vector<vk::AttachmentDescription>& val) {
    attachments = val;
    return *this;
  }

  RenderPass::Builder& RenderPass::Builder::addSubpass(const exqudens::vulkan::SubpassDescription& val) {
    subpasses.emplace_back(val);
    return *this;
  }

  RenderPass::Builder& RenderPass::Builder::setSubpasses(const std::vector<SubpassDescription>& val) {
    subpasses = val;
    return *this;
  }

  RenderPass::Builder& RenderPass::Builder::addDependency(const vk::SubpassDependency& val) {
    dependencies.emplace_back(val);
    return *this;
  }

  RenderPass::Builder& RenderPass::Builder::setDependencies(const std::vector<vk::SubpassDependency>& val) {
    dependencies = val;
    return *this;
  }

  RenderPass::Builder& RenderPass::Builder::setCreateInfo(const vk::RenderPassCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  RenderPass RenderPass::Builder::build() {
    try {
      RenderPass target = {};
      target.attachments = attachments;
      target.subpasses = subpasses;
      target.dependencies = dependencies;
      target.createInfo = createInfo.value_or(vk::RenderPassCreateInfo());
      target.createInfo.setAttachments(target.attachments);
      target.createInfo.setSubpassCount(target.subpasses.size());
      target.createInfo.setPSubpasses(target.subpasses.data());
      target.createInfo.setDependencies(target.dependencies);
      target.value = std::make_shared<vk::raii::RenderPass>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  RenderPass::Builder RenderPass::builder() {
    return {};
  }

}

#undef CALL_INFO
