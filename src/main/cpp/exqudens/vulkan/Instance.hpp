#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Instance {

        class Builder;

        VULKAN_HPP_NAMESPACE::ApplicationInfo applicationInfo;
        std::vector<const char*> enabledExtensionNames = {};
        std::vector<const char*> enabledLayerNames = {};
        VULKAN_HPP_NAMESPACE::InstanceCreateInfo createInfo;
        VULKAN_HPP_NAMESPACE::raii::Instance target = nullptr;

        static Builder builder(Instance& object);

        void submitDebugUtilsMessage(
            const VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCallbackDataEXT& callbackData,
            VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT messageTypes = VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
            VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity = VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
        );

        void submitDebugUtilsMessage(
            const std::string& message,
            VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT messageTypes = VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
            VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity = VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
        );

    };

    class EXQUDENS_VULKAN_EXPORT Instance::Builder {

        private:

            Instance& object;

        public:

            explicit Builder(Instance& object);

            Builder& setApplicationInfo(const VULKAN_HPP_NAMESPACE::ApplicationInfo& value);

            Builder& setEnabledExtensionNames(const std::vector<const char*>& value);

            Builder& setEnabledLayerNames(const std::vector<const char*>& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::InstanceCreateInfo& value);

            Instance& build(
                VULKAN_HPP_NAMESPACE::raii::Context& context
            );

    };
}

// implementation ---

#include <cstddef>
#include <cstdint>
#include <string>
#include <filesystem>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE Instance::Builder Instance::builder(Instance& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE Instance::Builder::Builder(Instance& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE void Instance::submitDebugUtilsMessage(
        const VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCallbackDataEXT& callbackData,
        VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT messageTypes,
        VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity
    ) {
        try {
            target.submitDebugUtilsMessageEXT(
                messageSeverity,
                messageTypes,
                callbackData
            );
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void Instance::submitDebugUtilsMessage(
        const std::string& message,
        VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT messageTypes,
        VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity
    ) {
        try {
            target.submitDebugUtilsMessageEXT(
                messageSeverity,
                messageTypes,
                VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCallbackDataEXT().setPMessage(message.c_str())
            );
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE Instance::Builder& Instance::Builder::setApplicationInfo(const VULKAN_HPP_NAMESPACE::ApplicationInfo& value) {
        object.applicationInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Instance::Builder& Instance::Builder::setEnabledExtensionNames(const std::vector<const char*>& value) {
        object.enabledExtensionNames = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Instance::Builder& Instance::Builder::setEnabledLayerNames(const std::vector<const char*>& value) {
        object.enabledLayerNames = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Instance::Builder& Instance::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::InstanceCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Instance& Instance::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Context& context
    ) {
        try {
            if (!object.enabledExtensionNames.empty()) {
                auto extensionProperties = context.enumerateInstanceExtensionProperties();
                for (size_t i = 0; i < object.enabledExtensionNames.size(); i++) {
                    std::string extensionName = object.enabledExtensionNames.at(i);
                    bool found = false;
                    for (size_t j = 0; j < extensionProperties.size(); j++) {
                        if (extensionName == extensionProperties.at(j).extensionName) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        throw std::runtime_error(CALL_INFO + ": Required extension not supported: '" + extensionName + "'");
                    }
                }
            }

            if (!object.enabledLayerNames.empty()) {
                auto layerProperties = context.enumerateInstanceLayerProperties();
                for (size_t i = 0; i < object.enabledLayerNames.size(); i++) {
                    std::string layerName = object.enabledLayerNames.at(i);
                    bool found = false;
                    for (size_t j = 0; j < layerProperties.size(); j++) {
                        if (layerName == layerProperties.at(j).layerName) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        throw std::runtime_error(CALL_INFO + ": Required layer not supported: '" + layerName + "'");
                    }
                }
            }

            object.createInfo.pApplicationInfo = &object.applicationInfo;
            object.createInfo.ppEnabledExtensionNames = object.enabledExtensionNames.data();
            object.createInfo.enabledExtensionCount = static_cast<uint32_t>(object.enabledExtensionNames.size());
            object.createInfo.ppEnabledLayerNames = object.enabledLayerNames.data();
            object.createInfo.enabledLayerCount = static_cast<uint32_t>(object.enabledLayerNames.size());
            object.target = context.createInstance(object.createInfo);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
