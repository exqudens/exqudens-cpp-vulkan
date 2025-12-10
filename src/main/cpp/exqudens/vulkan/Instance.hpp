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
        VULKAN_HPP_NAMESPACE::InstanceCreateInfo createInfo;

        static Builder builder();

    };

    class Instance::Builder {

        private:

            Instance resultObject = {};

        public:

            Builder& setApplicationInfo(const VULKAN_HPP_NAMESPACE::ApplicationInfo& value);

            Builder& setEnabledExtensionNames(const std::vector<const char*>& value);

            Builder& setEnabledLayerNames(const std::vector<const char*>& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::InstanceCreateInfo& value);

            Instance build(
                VULKAN_HPP_NAMESPACE::raii::Instance& instance,
                VULKAN_HPP_NAMESPACE::raii::Context& context
            );

    };

    // implementation ---

    inline Instance::Builder Instance::builder() {
        return {};
    }

    inline Instance::Builder& Instance::Builder::setApplicationInfo(const VULKAN_HPP_NAMESPACE::ApplicationInfo& value) {
        resultObject.applicationInfo = value;
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

    inline Instance::Builder& Instance::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::InstanceCreateInfo& value) {
        resultObject.createInfo = value;
        return *this;
    }


    inline Instance Instance::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Instance& instance,
        VULKAN_HPP_NAMESPACE::raii::Context& context
    ) {
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

            Instance result = {};
            result.applicationInfo = resultObject.applicationInfo;
            result.enabledExtensionNames = resultObject.enabledExtensionNames;
            result.enabledLayerNames = resultObject.enabledLayerNames;
            result.createInfo = resultObject.createInfo;
            result.createInfo.pApplicationInfo = &result.applicationInfo;
            result.createInfo.ppEnabledExtensionNames = result.enabledExtensionNames.data();
            result.createInfo.enabledExtensionCount = static_cast<uint32_t>(result.enabledExtensionNames.size());
            result.createInfo.ppEnabledLayerNames = result.enabledLayerNames.data();
            result.createInfo.enabledLayerCount = static_cast<uint32_t>(result.enabledLayerNames.size());
            instance = context.createInstance(result.createInfo);

            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
