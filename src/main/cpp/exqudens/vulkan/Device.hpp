#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Device {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    vk::DeviceCreateInfo createInfo;
    std::shared_ptr<vk::raii::Device> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Device& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Device::Builder {

    private:

      std::weak_ptr<vk::raii::PhysicalDevice> physicalDevice;
      std::optional<vk::DeviceCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Device::Builder& setPhysicalDevice(const std::weak_ptr<vk::raii::PhysicalDevice>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Device::Builder& setCreateInfo(const vk::DeviceCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Device build();

  };

}
