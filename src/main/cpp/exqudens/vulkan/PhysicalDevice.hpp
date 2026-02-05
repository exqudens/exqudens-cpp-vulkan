#pragma once

#include <cstddef>
#include <vector>
#include <functional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT PhysicalDevice {

        class Builder;

        std::vector<const char*> requiredExtensions = {};
        std::function<bool(
            size_t index,
            const VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& device,
            std::vector<const char*> requiredExtensions
        )> filterFunction = {};
        VULKAN_HPP_NAMESPACE::raii::PhysicalDevice target = nullptr;

        static Builder builder();

    };

    class EXQUDENS_VULKAN_EXPORT PhysicalDevice::Builder {

        private:

            PhysicalDevice resultObject = {};

        public:

            Builder& setRequiredExtensions(const std::vector<const char*>& value);

            Builder& setFilterFunction(const std::function<bool(size_t, const VULKAN_HPP_NAMESPACE::raii::PhysicalDevice&, std::vector<const char*>)>& value);

            PhysicalDevice& build(
                PhysicalDevice& physicalDevice,
                VULKAN_HPP_NAMESPACE::raii::Instance& instance
            );

    };
}

// implementation ---

#include <string>
#include <filesystem>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE PhysicalDevice::Builder PhysicalDevice::builder() {
        return {};
    }


    EXQUDENS_VULKAN_INLINE PhysicalDevice::Builder& PhysicalDevice::Builder::setRequiredExtensions(const std::vector<const char*>& value) {
        resultObject.requiredExtensions = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE PhysicalDevice::Builder& PhysicalDevice::Builder::setFilterFunction(
        const std::function<bool(size_t, const vk::raii::PhysicalDevice&, std::vector<const char*>)>& value
    ) {
        resultObject.filterFunction = value;
        return *this;
    }


    EXQUDENS_VULKAN_INLINE PhysicalDevice& PhysicalDevice::Builder::build(
        PhysicalDevice& physicalDevice,
        VULKAN_HPP_NAMESPACE::raii::Instance& instance
    ) {
        try {
            if (!resultObject.filterFunction) {
                resultObject.filterFunction = [](size_t, VULKAN_HPP_NAMESPACE::raii::PhysicalDevice, std::vector<const char*>) { return true; };
            }

            physicalDevice.requiredExtensions = resultObject.requiredExtensions;
            physicalDevice.filterFunction = resultObject.filterFunction;

            std::vector<vk::raii::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

            if (devices.empty()) {
                throw std::runtime_error(CALL_INFO + ": failed to find GPUs with Vulkan support");
            }

            for (size_t i = 0; i < devices.size(); i++) {
                VULKAN_HPP_NAMESPACE::raii::PhysicalDevice device = devices.at(i);
                bool found = physicalDevice.filterFunction(i, device, physicalDevice.requiredExtensions);
                if (found) {
                    physicalDevice.target = device;
                    break;
                }
            }

            return physicalDevice;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
