#pragma once

#include <optional>
#include <memory>
#include <functional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Image {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    vk::ImageCreateInfo createInfo;
    std::shared_ptr<vk::raii::Image> value;
    vk::MemoryPropertyFlags memoryCreateInfo;
    std::shared_ptr<vk::raii::DeviceMemory> memory;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Image& reference();

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::DeviceMemory& memoryReference();

  };

  class EXQUDENS_VULKAN_EXPORT Image::Builder {

    private:

      std::weak_ptr<vk::raii::PhysicalDevice> physicalDevice;
      std::weak_ptr<vk::raii::Device> device;
      std::function<uint32_t(
          vk::raii::PhysicalDevice&,
          const uint32_t&,
          const vk::MemoryPropertyFlags&
      )> memoryTypeIndexFunction;
      std::optional<vk::ImageCreateInfo> createInfo;
      std::optional<vk::MemoryPropertyFlags> memoryCreateInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Image::Builder& setPhysicalDevice(const std::weak_ptr<vk::raii::PhysicalDevice>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Image::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Image::Builder& setMemoryTypeIndexFunction(
          const std::function<uint32_t(
              vk::raii::PhysicalDevice&,
              const uint32_t&,
              const vk::MemoryPropertyFlags&
          )>& val
      );

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Image::Builder& setCreateInfo(const vk::ImageCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Image::Builder& setMemoryCreateInfo(const vk::MemoryPropertyFlags& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Image build();

  };

}
