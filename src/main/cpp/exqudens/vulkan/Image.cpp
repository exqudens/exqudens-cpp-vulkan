#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Image.hpp"
#include "exqudens/vulkan/Utility.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::Image& Image::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  vk::raii::DeviceMemory& Image::memoryReference() {
    try {
      if (!memory) {
        throw std::runtime_error(CALL_INFO + ": memory is not initialized!");
      }
      return *memory;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Image::Builder& Image::Builder::setPhysicalDevice(const std::weak_ptr<vk::raii::PhysicalDevice>& val) {
    physicalDevice = val;
    return *this;
  }

  Image::Builder& Image::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  Image::Builder& Image::Builder::setMemoryTypeIndexFunction(
      const std::function<uint32_t(
          vk::raii::PhysicalDevice&,
          const uint32_t&,
          const vk::MemoryPropertyFlags&
      )>& val
  ) {
    memoryTypeIndexFunction = val;
    return *this;
  }

  Image::Builder& Image::Builder::setCreateInfo(const vk::ImageCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  Image::Builder& Image::Builder::setMemoryCreateInfo(const vk::MemoryPropertyFlags& val) {
    memoryCreateInfo = val;
    return *this;
  }

  Image Image::Builder::build() {
    try {
      if (!memoryTypeIndexFunction) {
        memoryTypeIndexFunction = &Utility::memoryTypeIndex;
      }

      Image target = {};
      target.createInfo = createInfo.value();
      target.value = std::make_shared<vk::raii::Image>(
          *device.lock(),
          target.createInfo
      );
      target.memoryCreateInfo = memoryCreateInfo.value();
      vk::MemoryRequirements memoryRequirements = target.reference().getMemoryRequirements();
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

  Image::Builder Image::builder() {
    return {};
  }

}

#undef CALL_INFO
