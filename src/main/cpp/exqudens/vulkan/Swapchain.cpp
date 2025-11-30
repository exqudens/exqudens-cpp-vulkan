#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Swapchain.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::SwapchainKHR& Swapchain::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Swapchain::Builder& Swapchain::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  Swapchain::Builder& Swapchain::Builder::addGraphicsQueueFamilyIndex(const uint32_t& val) {
    addQueueFamilyIndex(val);
    return *this;
  }

  Swapchain::Builder& Swapchain::Builder::addPresentQueueFamilyIndex(const uint32_t& val) {
    addQueueFamilyIndex(val);
    return *this;
  }

  Swapchain::Builder& Swapchain::Builder::addQueueFamilyIndex(const uint32_t& val) {
    if (queueFamilyIndices.size() == 2) {
      throw std::runtime_error(CALL_INFO + ": queue family indices size equals 2!");
    }
    queueFamilyIndices.insert(val);
    return *this;
  }

  Swapchain::Builder& Swapchain::Builder::setQueueFamilyIndices(const std::vector<uint32_t>& val) {
    if (val.size() <= 2) {
      throw std::runtime_error(CALL_INFO + ": val size greater than 2!");
    }
    queueFamilyIndices = std::set<uint32_t>(val.begin(), val.end());
    return *this;
  }

  Swapchain::Builder& Swapchain::Builder::setCreateInfo(const vk::SwapchainCreateInfoKHR& val) {
    createInfo = val;
    return *this;
  }

  Swapchain Swapchain::Builder::build() {
    try {
      Swapchain target = {};
      target.queueFamilyIndices = std::vector<uint32_t>(queueFamilyIndices.begin(), queueFamilyIndices.end());
      target.createInfo = createInfo.value();

      if (target.queueFamilyIndices.size() == 2 && target.queueFamilyIndices[0] != target.queueFamilyIndices[1]) {
        target.createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        target.createInfo.setQueueFamilyIndices(target.queueFamilyIndices);
      } else {
        target.createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
      }

      target.value = std::make_shared<vk::raii::SwapchainKHR>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Swapchain::Builder Swapchain::builder() {
    return {};
  }

}

#undef CALL_INFO
