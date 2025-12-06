#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <filesystem>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct Instance {

        class Builder;

        VULKAN_HPP_NAMESPACE::ApplicationInfo applicationInfo;
        std::vector<const char*> enabledExtensionNames = {};
        std::vector<const char*> enabledLayerNames = {};
        VULKAN_HPP_NAMESPACE::InstanceCreateInfo instanceCreateInfo;
        VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
        VULKAN_HPP_NAMESPACE::raii::Instance targetObject = nullptr;
        VULKAN_HPP_NAMESPACE::raii::DebugUtilsMessengerEXT debugUtilsMessengerObject = nullptr;

        static Builder builder();

        VULKAN_HPP_NAMESPACE::raii::Instance& get();

    };

    class Instance::Builder {

        private:

            Instance resultObject = {};

        public:

            Builder& setApiVersion(const std::optional<uint32_t>& value);

            Builder& setApplicationName(const std::optional<const char*>& value);

            Builder& setApplicationVersion(const std::optional<uint32_t>& value);

            Builder& setEngineName(const std::optional<const char*>& value);

            Builder& setEngineVersion(const std::optional<uint32_t>& value);

            Builder& setEnabledExtensionNames(const std::vector<const char*>& value);

            Builder& setEnabledLayerNames(const std::vector<const char*>& value);

            Builder& setMessageSeverity(const std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagsEXT>& value);

            Builder& setMessageType(const std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT>& value);

            Builder& setDebugCallback(const std::optional<VULKAN_HPP_NAMESPACE::PFN_DebugUtilsMessengerCallbackEXT>& value);

            Instance build(VULKAN_HPP_NAMESPACE::raii::Context& context);

    };

// implementation ---

    inline Instance::Builder Instance::builder() {
        return {};
    }

    inline VULKAN_HPP_NAMESPACE::raii::Instance& Instance::get() {
        return targetObject;
    }

    inline Instance::Builder& Instance::Builder::setApiVersion(const std::optional<uint32_t>& value) {
        resultObject.applicationInfo.apiVersion = value.value_or(resultObject.applicationInfo.apiVersion);
        return *this;
    }

    inline Instance::Builder& Instance::Builder::setApplicationName(const std::optional<const char*>& value) {
        resultObject.applicationInfo.pApplicationName = value.value_or(resultObject.applicationInfo.pApplicationName);
        return *this;
    }

    inline Instance::Builder& Instance::Builder::setApplicationVersion(const std::optional<uint32_t>& value) {
        resultObject.applicationInfo.applicationVersion = value.value_or(resultObject.applicationInfo.applicationVersion);
        return *this;
    }

    inline Instance::Builder& Instance::Builder::setEngineName(const std::optional<const char*>& value) {
        resultObject.applicationInfo.pEngineName = value.value_or(resultObject.applicationInfo.pEngineName);
        return *this;
    }

    inline Instance::Builder& Instance::Builder::setEngineVersion(const std::optional<uint32_t>& value) {
        resultObject.applicationInfo.engineVersion = value.value_or(resultObject.applicationInfo.engineVersion);
        return *this;
    }

    inline Instance::Builder& Instance::Builder::setEnabledExtensionNames(const std::vector<const char*>& value) {
        resultObject.enabledExtensionNames = value;
        return *this;
    }

    inline Instance::Builder& Instance::Builder::setEnabledLayerNames(const std::vector<const char*>& value) {
        resultObject.enabledLayerNames = value;
        return *this;
    }

    inline Instance::Builder& Instance::Builder::setMessageSeverity(
        const std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagsEXT>& value
    ) {
        resultObject.debugUtilsMessengerCreateInfo.messageSeverity = value.value_or(resultObject.debugUtilsMessengerCreateInfo.messageSeverity);
        return *this;
    }

    inline Instance::Builder& Instance::Builder::setMessageType(
        const std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT>& value
    ) {
        resultObject.debugUtilsMessengerCreateInfo.messageType = value.value_or(resultObject.debugUtilsMessengerCreateInfo.messageType);
        return *this;
    }

    inline Instance::Builder& Instance::Builder::setDebugCallback(
        const std::optional<VULKAN_HPP_NAMESPACE::PFN_DebugUtilsMessengerCallbackEXT>& value
    ) {
        resultObject.debugUtilsMessengerCreateInfo.pfnUserCallback = value.value_or(resultObject.debugUtilsMessengerCreateInfo.pfnUserCallback);
        return *this;
    }

    inline Instance Instance::Builder::build(VULKAN_HPP_NAMESPACE::raii::Context& context) {
        try {
            if (!resultObject.enabledExtensionNames.empty()) {
                auto extensionProperties = context.enumerateInstanceExtensionProperties();
                for (size_t i = 0; i < resultObject.enabledExtensionNames.size(); i++) {
                    std::string extensionName = resultObject.enabledExtensionNames.at(i);
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

            if (!resultObject.enabledLayerNames.empty()) {
                auto layerProperties = context.enumerateInstanceLayerProperties();
                for (size_t i = 0; i < resultObject.enabledLayerNames.size(); i++) {
                    std::string layerName = resultObject.enabledLayerNames.at(i);
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

            Instance result;
            result.applicationInfo = resultObject.applicationInfo;
            result.enabledExtensionNames = resultObject.enabledExtensionNames;
            result.enabledLayerNames = resultObject.enabledLayerNames;
            result.instanceCreateInfo = resultObject.instanceCreateInfo;
            result.instanceCreateInfo.pApplicationInfo = &result.applicationInfo;
            result.instanceCreateInfo.ppEnabledExtensionNames = result.enabledExtensionNames.data();
            result.instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(result.enabledExtensionNames.size());
            result.instanceCreateInfo.ppEnabledLayerNames = result.enabledLayerNames.data();
            result.instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(result.enabledLayerNames.size());
            result.targetObject = context.createInstance(result.instanceCreateInfo);

            if (result.enabledLayerNames.empty()) {
                return result;
            }

            result.debugUtilsMessengerCreateInfo = resultObject.debugUtilsMessengerCreateInfo;
            result.debugUtilsMessengerObject = result.targetObject.createDebugUtilsMessengerEXT(result.debugUtilsMessengerCreateInfo);

            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
