#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  class EXQUDENS_VULKAN_EXPORT Utility {

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static void setEnvironmentVariable(const std::string& name, const std::string& value);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static std::optional<std::string> getEnvironmentVariable(const std::string& name);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static std::string toString(
          vk::DebugUtilsMessageSeverityFlagsEXT severity,
          vk::DebugUtilsMessageTypeFlagsEXT type,
          const std::string& message
      );

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static std::vector<char> readFileBytes(const std::string& path);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static vk::SwapchainCreateInfoKHR swapChainCreateInfo(
          vk::raii::PhysicalDevice& physicalDevice,
          vk::raii::SurfaceKHR& surface,
          const uint32_t& width,
          const uint32_t& height
      );

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static vk::Format imageDepthFormat(
          vk::raii::PhysicalDevice& physicalDevice,
          const std::vector<vk::Format>& formats,
          const vk::ImageTiling& tiling,
          const vk::FormatFeatureFlags& features
      );

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static vk::Format imageDepthFormat(
          vk::raii::PhysicalDevice& physicalDevice
      );

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static uint32_t memoryTypeIndex(
          vk::raii::PhysicalDevice& physicalDevice,
          const uint32_t& typeBits,
          const vk::MemoryPropertyFlags& requirementsMask
      );

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static vk::SampleCountFlagBits getMaxUsableSampleCount(
          const std::vector<vk::SampleCountFlags>& flags
      );

  };

}
