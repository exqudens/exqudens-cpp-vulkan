#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"
#include "exqudens/vulkan/Buffer.hpp"

namespace exqudens::vulkan {

  class EXQUDENS_VULKAN_EXPORT Utility {

    public:

      static void setEnvironmentVariable(const std::string& name, const std::string& value);

      static std::optional<std::string> getEnvironmentVariable(const std::string& name);

      static std::string toString(
          vk::DebugUtilsMessageSeverityFlagsEXT severity,
          vk::DebugUtilsMessageTypeFlagsEXT type,
          const std::string& message
      );

      static std::vector<char> readFile(const std::string& path);

      static vk::SwapchainCreateInfoKHR swapChainCreateInfo(
          vk::raii::PhysicalDevice& physicalDevice,
          vk::raii::SurfaceKHR& surface,
          const uint32_t& width,
          const uint32_t& height
      );

      static vk::Format imageDepthFormat(
          vk::raii::PhysicalDevice& physicalDevice,
          const std::vector<vk::Format>& formats,
          const vk::ImageTiling& tiling,
          const vk::FormatFeatureFlags& features
      );

      static vk::Format imageDepthFormat(
          vk::raii::PhysicalDevice& physicalDevice
      );

      static uint32_t memoryTypeIndex(
          vk::raii::PhysicalDevice& physicalDevice,
          const uint32_t& typeBits,
          const vk::MemoryPropertyFlags& requirementsMask
      );

      static vk::SampleCountFlagBits getMaxUsableSampleCount(
          const std::vector<vk::SampleCountFlags>& flags
      );

      static void copyTo(Buffer& buffer, const void* data);

  };

}
