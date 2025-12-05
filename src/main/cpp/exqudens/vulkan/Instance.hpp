#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <optional>
#include <vector>
#include <filesystem>
#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct Instance {

        class Builder;

        std::optional<uint32_t> apiVersion = VULKAN_HPP_NAMESPACE::ApiVersion14;
        std::optional<const char*> applicationName = "default-application-name";
        std::optional<uint32_t> applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        std::optional<const char*> engineName = "default-engine-name";
        std::optional<uint32_t> engineVersion = VK_MAKE_VERSION(0, 0, 1);
        VULKAN_HPP_NAMESPACE::ApplicationInfo applicationInfo = VULKAN_HPP_NAMESPACE::ApplicationInfo();
        std::vector<const char*> enabledExtensionNames = {};
        std::vector<const char*> enabledLayerNames = {};
        VULKAN_HPP_NAMESPACE::InstanceCreateInfo instanceCreateInfo = VULKAN_HPP_NAMESPACE::InstanceCreateInfo();
        std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagsEXT> messageSeverity = {
            VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eError
        };
        std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT> messageType = {
            VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
            //| VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        };
        std::optional<VULKAN_HPP_NAMESPACE::PFN_DebugUtilsMessengerCallbackEXT> debugCallback = {};
        VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT();
        std::weak_ptr<VULKAN_HPP_NAMESPACE::raii::Context> context = {};
        std::shared_ptr<VULKAN_HPP_NAMESPACE::raii::Instance> targetObject = {};
        std::shared_ptr<VULKAN_HPP_NAMESPACE::raii::DebugUtilsMessengerEXT> debugUtilsMessenger = {};

        inline static Builder builder();

        inline std::shared_ptr<VULKAN_HPP_NAMESPACE::raii::Instance> getPtr();

        inline VULKAN_HPP_NAMESPACE::raii::Instance& getRaii();

    };

    class Instance::Builder {

        private:

            Instance resultObject = {};

        public:

            inline Instance::Builder& setApiVersion(const std::optional<uint32_t>& value);

            inline Instance::Builder& setApplicationName(const std::optional<const char*>& value);

            inline Instance::Builder& setApplicationVersion(const std::optional<uint32_t>& value);

            inline Instance::Builder& setEngineName(const std::optional<const char*>& value);

            inline Instance::Builder& setEngineVersion(const std::optional<uint32_t>& value);

            inline Instance::Builder& setEnabledExtensionNames(const std::vector<const char*>& value);

            inline Instance::Builder& addEnabledExtensionNames(const std::vector<const char*>& value);

            inline Instance::Builder& addEnabledExtensionName(const char* value);

            inline Instance::Builder& setEnabledLayerNames(const std::vector<const char*>& value);

            inline Instance::Builder& addEnabledLayerNames(const std::vector<const char*>& value);

            inline Instance::Builder& addEnabledLayerName(const char* value);

            inline Instance::Builder& setMessageSeverity(const std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagsEXT>& value);

            inline Instance::Builder& setMessageType(const std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT>& value);

            inline Instance::Builder& setDebugCallback(const std::optional<VULKAN_HPP_NAMESPACE::PFN_DebugUtilsMessengerCallbackEXT>& value);

            inline Instance::Builder& setContext(const std::weak_ptr<VULKAN_HPP_NAMESPACE::raii::Context>& value);

            inline Instance build();

    };

    // implementation ---

    Instance::Builder Instance::builder() {
        return {};
    }

    std::shared_ptr<VULKAN_HPP_NAMESPACE::raii::Instance> Instance::getPtr() {
        return targetObject;
    }

    VULKAN_HPP_NAMESPACE::raii::Instance& Instance::getRaii() {
        try {
            if (!targetObject) {
                throw std::runtime_error(CALL_INFO + ": target object is not initialized!");
            }
            return *targetObject;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    Instance::Builder& Instance::Builder::setApiVersion(const std::optional<uint32_t>& value) {
        resultObject.apiVersion = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::setApplicationName(const std::optional<const char*>& value) {
        resultObject.applicationName = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::setApplicationVersion(const std::optional<uint32_t>& value) {
        resultObject.applicationVersion = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::setEngineName(const std::optional<const char*>& value) {
        resultObject.engineName = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::setEngineVersion(const std::optional<uint32_t>& value) {
        resultObject.engineVersion = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::setEnabledExtensionNames(const std::vector<const char*>& value) {
        resultObject.enabledExtensionNames = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::addEnabledExtensionNames(const std::vector<const char*>& value) {
        for (const auto& v : value) {
            resultObject.enabledExtensionNames.emplace_back(v);
        }
        return *this;
    }

    Instance::Builder& Instance::Builder::addEnabledExtensionName(const char* value) {
        resultObject.enabledExtensionNames.emplace_back(value);
        return *this;
    }

    Instance::Builder& Instance::Builder::setEnabledLayerNames(const std::vector<const char*>& value) {
        resultObject.enabledLayerNames = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::addEnabledLayerNames(const std::vector<const char*>& value) {
        for (const auto& v : value) {
            resultObject.enabledLayerNames.emplace_back(v);
        }
        return *this;
    }

    Instance::Builder& Instance::Builder::addEnabledLayerName(const char* value) {
        resultObject.enabledLayerNames.emplace_back(value);
        return *this;
    }

    Instance::Builder& Instance::Builder::setMessageSeverity(
        const std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagsEXT>& value
    ) {
        resultObject.messageSeverity = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::setMessageType(
        const std::optional<VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT>& value
    ) {
        resultObject.messageType = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::setDebugCallback(
        const std::optional<VULKAN_HPP_NAMESPACE::PFN_DebugUtilsMessengerCallbackEXT>& value
    ) {
        resultObject.debugCallback = value;
        return *this;
    }

    Instance::Builder& Instance::Builder::setContext(const std::weak_ptr<VULKAN_HPP_NAMESPACE::raii::Context>& value) {
        resultObject.context = value;
        return *this;
    }

    Instance Instance::Builder::build() {
        try {
            if (resultObject.context.expired()) {
                throw std::runtime_error(CALL_INFO + ": context is not initialized!");
            }

            auto extensionProperties = resultObject.context.lock()->enumerateInstanceExtensionProperties();
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

            auto layerProperties = resultObject.context.lock()->enumerateInstanceLayerProperties();
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

            if (!resultObject.enabledLayerNames.empty() && !resultObject.debugCallback.has_value()) {
                throw std::runtime_error(CALL_INFO + ": debug callback is not initialized!");
            }

            Instance result = resultObject;
            result.applicationInfo = VULKAN_HPP_NAMESPACE::ApplicationInfo();
            result.applicationInfo.apiVersion = result.apiVersion.value();
            result.applicationInfo.pApplicationName = result.applicationName.value();
            result.applicationInfo.applicationVersion = result.applicationVersion.value();
            result.applicationInfo.pEngineName = result.engineName.value();
            result.applicationInfo.engineVersion = result.engineVersion.value();
            result.instanceCreateInfo = VULKAN_HPP_NAMESPACE::InstanceCreateInfo();
            result.instanceCreateInfo.pApplicationInfo = &result.applicationInfo;
            result.instanceCreateInfo.ppEnabledExtensionNames = result.enabledExtensionNames.data();
            result.instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(result.enabledExtensionNames.size());
            result.instanceCreateInfo.ppEnabledLayerNames = result.enabledLayerNames.data();
            result.instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(result.enabledLayerNames.size());
            result.targetObject = std::make_shared<VULKAN_HPP_NAMESPACE::raii::Instance>(
                *result.context.lock(),
                result.instanceCreateInfo
            );
            if (result.enabledLayerNames.empty()) {
                return result;
            }
            result.debugUtilsMessengerCreateInfo = VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT();
            result.debugUtilsMessengerCreateInfo.messageSeverity = result.messageSeverity.value();
            result.debugUtilsMessengerCreateInfo.messageType = result.messageType.value();
            result.debugUtilsMessengerCreateInfo.pfnUserCallback = result.debugCallback.value();
            result.debugUtilsMessenger = std::make_shared<VULKAN_HPP_NAMESPACE::raii::DebugUtilsMessengerEXT>(
                result.getRaii(),
                result.debugUtilsMessengerCreateInfo
            );
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
