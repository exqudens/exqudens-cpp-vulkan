#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Device {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    vk::DeviceCreateInfo createInfo;
    std::shared_ptr<vk::raii::Device> value;

    vk::raii::Device& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Device::Builder {

    private:

      std::weak_ptr<vk::raii::PhysicalDevice> physicalDevice;
      std::optional<vk::DeviceCreateInfo> createInfo;

    public:

      Device::Builder& setPhysicalDevice(const std::weak_ptr<vk::raii::PhysicalDevice>& val);

      Device::Builder& setCreateInfo(const vk::DeviceCreateInfo& val);

      Device build();

  };

}
