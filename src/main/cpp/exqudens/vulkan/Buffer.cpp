#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Buffer.hpp"
#include "exqudens/vulkan/Utility.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::Buffer& Buffer::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  vk::raii::DeviceMemory& Buffer::memoryReference() {
    try {
      if (!memory) {
        throw std::runtime_error(CALL_INFO + ": memory is not initialized!");
      }
      return *memory;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void Buffer::fill(const void* data, size_t offset, size_t size, vk::MemoryMapFlags flags) {
    try {
      void* tmpData = memoryReference().mapMemory(offset, size, flags);
      std::memcpy(tmpData, data, size);
      memoryReference().unmapMemory();
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void Buffer::fill(const void* data) {
    try {
      void* tmpData = memoryReference().mapMemory(0, createInfo.size, {});
      std::memcpy(tmpData, data, static_cast<size_t>(createInfo.size));
      memoryReference().unmapMemory();
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Buffer::Builder& Buffer::Builder::setPhysicalDevice(const std::weak_ptr<vk::raii::PhysicalDevice>& val) {
    physicalDevice = val;
    return *this;
  }

  Buffer::Builder& Buffer::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  Buffer::Builder& Buffer::Builder::setMemoryTypeIndexFunction(
      const std::function<uint32_t(
          vk::raii::PhysicalDevice&,
          const uint32_t&,
          const vk::MemoryPropertyFlags&
      )>& val
  ) {
    memoryTypeIndexFunction = val;
    return *this;
  }

  Buffer::Builder& Buffer::Builder::setCreateInfo(const vk::BufferCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  Buffer::Builder& Buffer::Builder::setMemoryCreateInfo(const vk::MemoryPropertyFlags& val) {
    memoryCreateInfo = val;
    return *this;
  }

  Buffer Buffer::Builder::build() {
    try {
      if (!memoryTypeIndexFunction) {
        memoryTypeIndexFunction = &Utility::memoryTypeIndex;
      }

      Buffer target = {};
      target.createInfo = createInfo.value();
      target.value = std::make_shared<vk::raii::Buffer>(
          *device.lock(),
          target.createInfo
      );
      vk::MemoryRequirements memoryRequirements = target.reference().getMemoryRequirements();
      target.memoryCreateInfo = memoryCreateInfo.value();
      uint32_t memoryType = memoryTypeIndexFunction(
          *physicalDevice.lock(),
          memoryRequirements.memoryTypeBits,
          target.memoryCreateInfo
      );
      target.memory = std::make_shared<vk::raii::DeviceMemory>(
          *device.lock(),
          vk::MemoryAllocateInfo()
              .setAllocationSize(memoryRequirements.size)
              .setMemoryTypeIndex(memoryType)
      );
      target.reference().bindMemory(*target.memoryReference(), 0);
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Buffer::Builder Buffer::builder() {
    return {};
  }

}

#undef CALL_INFO
