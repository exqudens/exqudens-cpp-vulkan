#pragma once

#include <optional>
#include <vector>
#include <set>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Swapchain {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    std::vector<uint32_t> queueFamilyIndices;
    vk::SwapchainCreateInfoKHR createInfo;
    std::shared_ptr<vk::raii::SwapchainKHR> value;

    vk::raii::SwapchainKHR& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Swapchain::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::set<uint32_t> queueFamilyIndices;
      std::optional<vk::SwapchainCreateInfoKHR> createInfo;

    public:

      Swapchain::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      Swapchain::Builder& addGraphicsQueueFamilyIndex(const uint32_t& val);

      Swapchain::Builder& addPresentQueueFamilyIndex(const uint32_t& val);

      Swapchain::Builder& addQueueFamilyIndex(const uint32_t& val);

      Swapchain::Builder& setQueueFamilyIndices(const std::vector<uint32_t>& val);

      Swapchain::Builder& setCreateInfo(const vk::SwapchainCreateInfoKHR& val);

      Swapchain build();

  };

}
