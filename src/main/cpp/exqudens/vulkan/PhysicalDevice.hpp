#pragma once

#include <string>
#include <optional>
#include <vector>
#include <memory>
#include <functional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"
#include "exqudens/vulkan/QueueInfo.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT PhysicalDevice {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    std::vector<const char*> enabledExtensionNames;
    std::vector<QueueInfo> queueInfos;
    std::vector<float> queuePriorities;
    std::vector<vk::DeviceQueueCreateInfo> computeQueueCreateInfos;
    std::vector<vk::DeviceQueueCreateInfo> transferQueueCreateInfos;
    std::vector<vk::DeviceQueueCreateInfo> graphicsQueueCreateInfos;
    std::vector<vk::DeviceQueueCreateInfo> presentQueueCreateInfos;
    std::vector<vk::DeviceQueueCreateInfo> uniqueQueueCreateInfos;
    std::shared_ptr<vk::raii::PhysicalDevice> value;

    vk::raii::PhysicalDevice& reference();

  };

  class EXQUDENS_VULKAN_EXPORT PhysicalDevice::Builder {

    private:

      std::weak_ptr<vk::raii::Instance> instance;
      std::vector<const char*> enabledExtensionNames;
      std::optional<vk::SurfaceKHR> surface;
      std::vector<float> queuePriorities;
      std::function<bool(const vk::raii::PhysicalDevice&)> isSuitableFunction;

    public:

      PhysicalDevice::Builder& setInstance(const std::weak_ptr<vk::raii::Instance>& val);

      PhysicalDevice::Builder& addEnabledExtensionName(const char* val);

      PhysicalDevice::Builder& addEnabledExtensionName(const std::string& val);

      PhysicalDevice::Builder& setEnabledExtensionNames(const std::vector<const char*>& val);

      PhysicalDevice::Builder& setSurface(const vk::SurfaceKHR& val);

      PhysicalDevice::Builder& setSurface(const std::weak_ptr<vk::raii::SurfaceKHR>& val);

      PhysicalDevice::Builder& addQueuePriority(const float& val);

      PhysicalDevice::Builder& setQueuePriorities(const std::vector<float>& val);

      PhysicalDevice::Builder& setIsSuitableFunction(const std::function<bool(const vk::raii::PhysicalDevice&)>& val);

      PhysicalDevice build();

  };

}
