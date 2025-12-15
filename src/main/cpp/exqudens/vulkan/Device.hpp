#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct Device {

        class Builder;

        std::vector<VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo> queueCreateInfos = {};
        VULKAN_HPP_NAMESPACE::DeviceCreateInfo createInfo;
        VULKAN_HPP_NAMESPACE::raii::Device target = nullptr;

        static Builder builder();

    };

    class Device::Builder {

        private:

            Device resultObject = {};

        public:

            Builder& setQueueCreateInfos(const std::vector<VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo>& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::DeviceCreateInfo& value);

            Device& build(
                Device& device,
                VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice
            );

    };

    // implementation ---

    inline Device::Builder Device::builder() {
        return {};
    }

    inline Device::Builder& Device::Builder::setQueueCreateInfos(const std::vector<VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo>& value) {
        resultObject.queueCreateInfos = value;
        return *this;
    }

    inline Device::Builder& Device::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::DeviceCreateInfo& value) {
        resultObject.createInfo = value;
        return *this;
    }

    inline Device& Device::Builder::build(
        Device& device,
        VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice
    ) {
        try {
            device.queueCreateInfos = resultObject.queueCreateInfos;
            device.createInfo = resultObject.createInfo;
            device.createInfo.queueCreateInfoCount = static_cast<uint32_t>(device.queueCreateInfos.size());
            device.createInfo.pQueueCreateInfos = device.queueCreateInfos.data();
            device.target = physicalDevice.createDevice(device.createInfo);

            return device;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
