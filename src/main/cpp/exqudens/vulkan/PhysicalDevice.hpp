#pragma once

//#include <cstdint>
#include <cstddef>
#include <string>
//#include <optional>
#include <vector>
#include <filesystem>
#include <functional>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct PhysicalDevice {

        class Builder;

        std::vector<const char*> requiredExtensions = {};
        std::function<bool(
            size_t index,
            VULKAN_HPP_NAMESPACE::raii::PhysicalDevice device,
            std::vector<const char*> requiredExtensions
        )> filterFunction = {};
        VULKAN_HPP_NAMESPACE::raii::PhysicalDevice targetObject = nullptr;

        static Builder builder();

        VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& get();

    };

    class PhysicalDevice::Builder {

        private:

            PhysicalDevice resultObject = {};

        public:

            Builder& setRequiredExtensions(const std::vector<const char*>& value);

            Builder& setFilterFunction(const std::function<bool(size_t, VULKAN_HPP_NAMESPACE::raii::PhysicalDevice, std::vector<const char*>)>& value);

            PhysicalDevice build(VULKAN_HPP_NAMESPACE::raii::Instance& instance);

    };

// implementation ---

    inline PhysicalDevice::Builder PhysicalDevice::builder() {
        return {};
    }

    inline VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& PhysicalDevice::get() {
        return targetObject;
    }

    inline PhysicalDevice::Builder& PhysicalDevice::Builder::setRequiredExtensions(const std::vector<const char*>& value) {
        resultObject.requiredExtensions = value;
        return *this;
    }

    inline PhysicalDevice::Builder& PhysicalDevice::Builder::setFilterFunction(
        const std::function<bool(size_t, vk::raii::PhysicalDevice, std::vector<const char*>)>& value
    ) {
        resultObject.filterFunction = value;
        return *this;
    }


    inline PhysicalDevice PhysicalDevice::Builder::build(VULKAN_HPP_NAMESPACE::raii::Instance& instance) {
        try {
            if (!resultObject.filterFunction) {
                resultObject.filterFunction = [](size_t, VULKAN_HPP_NAMESPACE::raii::PhysicalDevice, std::vector<const char*>) { return true; };
            }

            PhysicalDevice result;
            result.requiredExtensions = resultObject.requiredExtensions;
            result.filterFunction = resultObject.filterFunction;

            std::vector<vk::raii::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

            if (devices.empty()) {
                throw std::runtime_error(CALL_INFO + ": failed to find GPUs with Vulkan support");
            }

            for (size_t i = 0; i < devices.size(); i++) {
                VULKAN_HPP_NAMESPACE::raii::PhysicalDevice device = devices.at(i);
                bool found = result.filterFunction(i, device, result.requiredExtensions);
                if (found) {
                    result.targetObject = device;
                    break;
                }
            }

            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }


}

#undef CALL_INFO
