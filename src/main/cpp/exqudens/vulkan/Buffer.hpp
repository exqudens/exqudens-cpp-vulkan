#pragma once

#include <optional>
#include <memory>
#include <functional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Buffer {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    vk::BufferCreateInfo createInfo;
    std::shared_ptr<vk::raii::Buffer> value;
    vk::MemoryPropertyFlags memoryCreateInfo;
    std::shared_ptr<vk::raii::DeviceMemory> memory;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Buffer& reference();

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::DeviceMemory& memoryReference();

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES void fill(const void* data, size_t offset, size_t size, vk::MemoryMapFlags flags);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES void fill(const void* data);

  };

  class EXQUDENS_VULKAN_EXPORT Buffer::Builder {

    private:

      std::weak_ptr<vk::raii::PhysicalDevice> physicalDevice;
      std::weak_ptr<vk::raii::Device> device;
      std::function<uint32_t(
          vk::raii::PhysicalDevice&,
          const uint32_t&,
          const vk::MemoryPropertyFlags&
      )> memoryTypeIndexFunction;
      std::optional<vk::BufferCreateInfo> createInfo;
      std::optional<vk::MemoryPropertyFlags> memoryCreateInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Buffer::Builder& setPhysicalDevice(const std::weak_ptr<vk::raii::PhysicalDevice>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Buffer::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Buffer::Builder& setMemoryTypeIndexFunction(
          const std::function<uint32_t(
              vk::raii::PhysicalDevice&,
              const uint32_t&,
              const vk::MemoryPropertyFlags&
          )>& val
      );

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Buffer::Builder& setCreateInfo(const vk::BufferCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Buffer::Builder& setMemoryCreateInfo(const vk::MemoryPropertyFlags& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Buffer build();

  };

}
