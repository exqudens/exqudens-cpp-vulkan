#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <optional>
#include <vector>
#include <fstream>
#include <stdexcept>

#include <vulkan/vulkan.hpp>

#include "exqudens/vulkan/Macros.hpp"

namespace exqudens::vulkan {

  class Utility {

    public:

      static void setEnvironmentVariable(const std::string& name, const std::string& value) {
        try {
#if defined(_WINDOWS)
          _putenv_s(name.c_str(), value.c_str());
#endif
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      static std::optional<std::string> getEnvironmentVariable(const std::string& name) {
        try {
          std::optional<std::string> value;
#if defined(_WINDOWS)
          char* buffer;
          size_t size;
          errno_t error = _dupenv_s(&buffer, &size, name.c_str());
          if (error) {
            return value;
          }
          if (buffer != nullptr) {
            value.emplace(std::string(buffer));
          }
#endif
          return value;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      static std::string toString(
          vk::DebugUtilsMessageSeverityFlagsEXT severity,
          vk::DebugUtilsMessageTypeFlagsEXT type,
          const std::string& message
      ) {
        try {
          return vk::to_string(severity) + " " + vk::to_string(type) + ": " + message;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      static std::vector<char> readFile(const std::string& path) {
        try {
          std::ifstream file(path, std::ios::ate | std::ios::binary);

          if (!file.is_open()) {
            throw std::runtime_error(CALL_INFO() + ": failed to open file: '" + path + "'!");
          }

          std::streamsize fileSize = file.tellg();
          std::vector<char> buffer(fileSize);

          file.seekg(0);
          file.read(buffer.data(), fileSize);

          file.close();

          return buffer;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      static vk::SwapchainCreateInfoKHR swapChainCreateInfo(
          vk::raii::PhysicalDevice& physicalDevice,
          vk::raii::SurfaceKHR& surface,
          const uint32_t& width,
          const uint32_t& height
      ) {
        try {
          std::optional<vk::SurfaceFormatKHR> surfaceFormat;
          std::optional<vk::PresentModeKHR> surfacePresentMode;
          std::optional<vk::SurfaceCapabilitiesKHR> surfaceCapabilities;
          std::optional<vk::Extent2D> surfaceExtent;
          std::optional<vk::SurfaceTransformFlagBitsKHR> surfaceTransform;
          std::optional<vk::CompositeAlphaFlagBitsKHR> surfaceCompositeAlpha;

          std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
          if (surfaceFormats.size() == 1 && surfaceFormats.front() == vk::Format::eUndefined) {
            surfaceFormat = surfaceFormats.front();
          } else {
            for (const vk::SurfaceFormatKHR& f : surfaceFormats) {
              if (vk::Format::eB8G8R8A8Srgb == f.format && vk::ColorSpaceKHR::eSrgbNonlinear == f.colorSpace) {
                surfaceFormat = f;
                break;
              }
            }
          }

          std::vector<vk::PresentModeKHR> surfacePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
          surfacePresentMode = vk::PresentModeKHR::eFifo;
          for (const vk::PresentModeKHR& p : surfacePresentModes) {
            if (vk::PresentModeKHR::eMailbox == p) {
              surfacePresentMode = p;
              break;
            }
          }

          surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);

          if (surfaceCapabilities.value().currentExtent.width == std::numeric_limits<uint32_t>::max()) {
            surfaceExtent = vk::Extent2D()
                .setWidth(std::clamp(width, surfaceCapabilities.value().minImageExtent.width, surfaceCapabilities.value().maxImageExtent.width))
                .setHeight(std::clamp(height, surfaceCapabilities.value().minImageExtent.height, surfaceCapabilities.value().maxImageExtent.height));
          } else {
            surfaceExtent = surfaceCapabilities.value().currentExtent;
          }

          if (surfaceCapabilities.value().supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
            surfaceTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
          } else {
            surfaceTransform = surfaceCapabilities.value().currentTransform;
          }

          if (surfaceCapabilities.value().supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
            surfaceCompositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
          } else if (surfaceCapabilities.value().supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
            surfaceCompositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePostMultiplied;
          } else if (surfaceCapabilities.value().supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) {
            surfaceCompositeAlpha = vk::CompositeAlphaFlagBitsKHR::eInherit;
          } else {
            surfaceCompositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
          }

          return vk::SwapchainCreateInfoKHR()
              .setFlags({})
              .setSurface(*surface)
              .setMinImageCount(surfaceCapabilities.value().minImageCount)
              .setImageFormat(surfaceFormat.value().format)
              .setImageColorSpace(surfaceFormat.value().colorSpace)
              .setImageExtent(surfaceExtent.value())
              .setImageArrayLayers(1)
              .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
              .setPreTransform(surfaceTransform.value())
              .setCompositeAlpha(surfaceCompositeAlpha.value())
              .setPresentMode(surfacePresentMode.value())
              .setClipped(true)
              .setOldSwapchain({});
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      static vk::Format imageDepthFormat(
          vk::raii::PhysicalDevice& physicalDevice,
          const std::vector<vk::Format>& formats,
          const vk::ImageTiling& tiling,
          const vk::FormatFeatureFlags& features
      ) {
        try {
          for (const vk::Format& format : formats) {
            vk::FormatProperties properties = physicalDevice.getFormatProperties(format);
            if (vk::ImageTiling::eLinear ==  tiling && (properties.linearTilingFeatures & features) == features) {
              return format;
            } else if (vk::ImageTiling::eOptimal == tiling && (properties.optimalTilingFeatures & features) == features) {
              return format;
            }
          }
          throw std::runtime_error(CALL_INFO() + ": failed to find image depth format!");
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      static vk::Format imageDepthFormat(
          vk::raii::PhysicalDevice& physicalDevice
      ) {
        try {
          return imageDepthFormat(
              physicalDevice,
              {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
              vk::ImageTiling::eOptimal,
              vk::FormatFeatureFlagBits::eDepthStencilAttachment
          );
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      static uint32_t memoryTypeIndex(
          vk::raii::PhysicalDevice& physicalDevice,
          const uint32_t& typeBits,
          const vk::MemoryPropertyFlags& requirementsMask
      ) {
        try {
          vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();
          for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
            if (
                (typeBits & 1)
                && ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
                ) {
              return i;
            }
          }
          throw std::runtime_error(CALL_INFO() + ": failed to find memory type index!");
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      static vk::SampleCountFlagBits getMaxUsableSampleCount(
          const std::vector<vk::SampleCountFlags>& flags
      ) {
        try {
          vk::SampleCountFlags counts = vk::SampleCountFlagBits::e1;
          if (!flags.empty()) {
            counts = flags.front();
            for (size_t i = 1; i < flags.size(); i++) {
              counts &= flags[i];
            }
          }
          if (counts & vk::SampleCountFlagBits::e64) {
            return vk::SampleCountFlagBits::e64;
          } else if (counts & vk::SampleCountFlagBits::e32) {
            return vk::SampleCountFlagBits::e32;
          } else if (counts & vk::SampleCountFlagBits::e16) {
            return vk::SampleCountFlagBits::e16;
          } else if (counts & vk::SampleCountFlagBits::e8) {
            return vk::SampleCountFlagBits::e8;
          } else if (counts & vk::SampleCountFlagBits::e4) {
            return vk::SampleCountFlagBits::e4;
          } else if (counts & vk::SampleCountFlagBits::e2) {
            return vk::SampleCountFlagBits::e2;
          } else {
            return vk::SampleCountFlagBits::e1;
          }
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

}
