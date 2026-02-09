#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT PhysicalDevice {

        class Builder;

        std::vector<const char*> requiredExtensions = {};
        VULKAN_HPP_NAMESPACE::raii::PhysicalDevice target = nullptr;

        static Builder builder(PhysicalDevice& object);

    };

    class EXQUDENS_VULKAN_EXPORT PhysicalDevice::Builder {

        private:

            PhysicalDevice& object;

        public:

            explicit Builder(PhysicalDevice& object);

            Builder& setRequiredExtensions(const std::vector<const char*>& value);

            PhysicalDevice& build(
                VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice
            );

    };
}

// implementation ---

#include <string>
#include <filesystem>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE PhysicalDevice::Builder PhysicalDevice::builder(PhysicalDevice& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE PhysicalDevice::Builder::Builder(PhysicalDevice& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE PhysicalDevice::Builder& PhysicalDevice::Builder::setRequiredExtensions(const std::vector<const char*>& value) {
        object.requiredExtensions = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE PhysicalDevice& PhysicalDevice::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice
    ) {
        try {
            object.target = physicalDevice;
            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
