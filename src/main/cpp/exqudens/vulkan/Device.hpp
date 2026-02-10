#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Device {

        class Builder;

        std::vector<VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo> queueCreateInfos = {};
        VULKAN_HPP_NAMESPACE::DeviceCreateInfo createInfo;
        VULKAN_HPP_NAMESPACE::raii::Device target = nullptr;

        static Builder builder(Device& object);

    };

    class EXQUDENS_VULKAN_EXPORT Device::Builder {

        private:

            Device& object;

        public:

            explicit Builder(Device& object);

            Builder& setQueueCreateInfos(const std::vector<VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo>& value);

            Builder& addQueueCreateInfos(const VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo& value);

            Builder& addUniqQueueCreateInfos(const VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::DeviceCreateInfo& value);

            Device& build(
                VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice
            );

    };
}

// implementation ---

#include <cstdint>
#include <string>
#include <filesystem>
#include <stdexcept>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE Device::Builder Device::builder(Device& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE Device::Builder::Builder(Device& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE Device::Builder& Device::Builder::setQueueCreateInfos(const std::vector<VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo>& value) {
        object.queueCreateInfos = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Device::Builder& Device::Builder::addQueueCreateInfos(const VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo& value) {
        object.queueCreateInfos.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Device::Builder& Device::Builder::addUniqQueueCreateInfos(const VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo& value) {
        bool containsQueueCreateInfos = false;
        for (const auto& v : object.queueCreateInfos) {
            if (v.queueFamilyIndex == value.queueFamilyIndex) {
                containsQueueCreateInfos = true;
                break;
            }
        }
        if (!containsQueueCreateInfos) {
            object.queueCreateInfos.emplace_back(value);
        }
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Device::Builder& Device::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::DeviceCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Device& Device::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice
    ) {
        try {
            object.createInfo.queueCreateInfoCount = static_cast<uint32_t>(object.queueCreateInfos.size());
            object.createInfo.pQueueCreateInfos = object.queueCreateInfos.data();
            object.target = physicalDevice.createDevice(object.createInfo);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
