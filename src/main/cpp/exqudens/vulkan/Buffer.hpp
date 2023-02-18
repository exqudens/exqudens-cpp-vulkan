#pragma once

#include <optional>
#include <memory>
#include <functional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Buffer {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    vk::BufferCreateInfo createInfo;
    std::shared_ptr<vk::raii::Buffer> value;
    vk::MemoryPropertyFlags memoryCreateInfo;
    std::shared_ptr<vk::raii::DeviceMemory> memory;

    vk::raii::Buffer& reference();

    vk::raii::DeviceMemory& memoryReference();

    void fill(const void* data, size_t offset, size_t size, vk::MemoryMapFlags flags);

    void fill(const void* data);

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

      Buffer::Builder& setPhysicalDevice(const std::weak_ptr<vk::raii::PhysicalDevice>& val);

      Buffer::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      Buffer::Builder& setMemoryTypeIndexFunction(
          const std::function<uint32_t(
              vk::raii::PhysicalDevice&,
              const uint32_t&,
              const vk::MemoryPropertyFlags&
          )>& val
      );

      Buffer::Builder& setCreateInfo(const vk::BufferCreateInfo& val);

      Buffer::Builder& setMemoryCreateInfo(const vk::MemoryPropertyFlags& val);

      Buffer build();

  };

}
