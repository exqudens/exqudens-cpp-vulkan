#pragma once

#include <optional>
#include <memory>
#include <functional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Image {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    vk::ImageCreateInfo createInfo;
    std::shared_ptr<vk::raii::Image> value;
    vk::MemoryPropertyFlags memoryCreateInfo;
    std::shared_ptr<vk::raii::DeviceMemory> memory;

    vk::raii::Image& reference();

    vk::raii::DeviceMemory& memoryReference();

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

      Image::Builder& setPhysicalDevice(const std::weak_ptr<vk::raii::PhysicalDevice>& val);

      Image::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      Image::Builder& setMemoryTypeIndexFunction(
          const std::function<uint32_t(
              vk::raii::PhysicalDevice&,
              const uint32_t&,
              const vk::MemoryPropertyFlags&
          )>& val
      );

      Image::Builder& setCreateInfo(const vk::ImageCreateInfo& val);

      Image::Builder& setMemoryCreateInfo(const vk::MemoryPropertyFlags& val);

      Image build();

  };

}
