#pragma once

#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT DeviceMemory {

        class Builder;

        std::optional<VULKAN_HPP_NAMESPACE::MemoryAllocateInfo> allocateInfo = {};
        VULKAN_HPP_NAMESPACE::raii::DeviceMemory target = nullptr;

        static VULKAN_HPP_NAMESPACE::MemoryAllocateInfo allocateInfoFrom(
            VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice,
            VULKAN_HPP_NAMESPACE::raii::Buffer& buffer,
            const VULKAN_HPP_NAMESPACE::MemoryPropertyFlags& flags
        );

        static Builder builder(DeviceMemory& object);

        void fill(
            const void* data,
            std::optional<VULKAN_HPP_NAMESPACE::DeviceSize> offset = {},
            std::optional<VULKAN_HPP_NAMESPACE::DeviceSize> size = {},
            std::optional<VULKAN_HPP_NAMESPACE::MemoryMapFlags> flags = {}
        );

        void clear();

        void clearAndRelease();

    };

    class EXQUDENS_VULKAN_EXPORT DeviceMemory::Builder {

        private:

            DeviceMemory& object;

        public:

            explicit Builder(DeviceMemory& object);

            Builder& setAllocateInfo(const VULKAN_HPP_NAMESPACE::MemoryAllocateInfo& value);

            DeviceMemory& build(
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };

}

// implementation ---

#include <cstring>
#include <string>
#include <filesystem>
#include <stdexcept>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE VULKAN_HPP_NAMESPACE::MemoryAllocateInfo DeviceMemory::allocateInfoFrom(
        VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice,
        VULKAN_HPP_NAMESPACE::raii::Buffer& buffer,
        const VULKAN_HPP_NAMESPACE::MemoryPropertyFlags& flags
    ) {
        try {
            VULKAN_HPP_NAMESPACE::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();
            VULKAN_HPP_NAMESPACE::MemoryRequirements memoryRequirements = buffer.getMemoryRequirements();

            for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {
                if (
                    (memoryRequirements.memoryTypeBits & (1 << i))
                    && (physicalDeviceMemoryProperties.memoryTypes.at(i).propertyFlags & flags) == flags
                ) {
                    VULKAN_HPP_NAMESPACE::MemoryAllocateInfo result;
                    result.allocationSize = memoryRequirements.size;
                    result.memoryTypeIndex = i;
                    return result;
                }
            }

            throw std::runtime_error(CALL_INFO + ": failed to find suitable memory type!");
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE DeviceMemory::Builder DeviceMemory::builder(DeviceMemory& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE void DeviceMemory::fill(
        const void* data,
        std::optional<VULKAN_HPP_NAMESPACE::DeviceSize> offset,
        std::optional<VULKAN_HPP_NAMESPACE::DeviceSize> size,
        std::optional<VULKAN_HPP_NAMESPACE::MemoryMapFlags> flags
    ) {
        try {
            if (!offset.has_value()) {
                offset = 0;
            }

            if (!size.has_value()) {
                size = allocateInfo.value().allocationSize;
            }

            if (!flags.has_value()) {
                flags = VULKAN_HPP_NAMESPACE::MemoryMapFlags();
            }

            void* tmpData = target.mapMemory(offset.value(), size.value(), flags.value());
            std::memcpy(tmpData, data, size.value());
            target.unmapMemory();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void DeviceMemory::clear() {
        try {
            allocateInfo.reset();
            target.clear();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void DeviceMemory::clearAndRelease() {
        try {
            clear();
            target.release();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE DeviceMemory::Builder::Builder(DeviceMemory& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE DeviceMemory::Builder& DeviceMemory::Builder::setAllocateInfo(const VULKAN_HPP_NAMESPACE::MemoryAllocateInfo& value) {
        object.allocateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DeviceMemory& DeviceMemory::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.allocateInfo.has_value()) {
                object.allocateInfo = VULKAN_HPP_NAMESPACE::MemoryAllocateInfo();
            }

            object.target = device.allocateMemory(object.allocateInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
