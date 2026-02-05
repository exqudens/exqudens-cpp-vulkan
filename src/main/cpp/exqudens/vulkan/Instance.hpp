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

        static Builder builder();

    };

    class EXQUDENS_VULKAN_EXPORT Instance::Builder {

        private:

            Instance resultObject = {};

        public:

            Builder& setApplicationInfo(const VULKAN_HPP_NAMESPACE::ApplicationInfo& value);

            Builder& setEnabledExtensionNames(const std::vector<const char*>& value);

            Builder& setEnabledLayerNames(const std::vector<const char*>& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::InstanceCreateInfo& value);

            Instance& build(
                Instance& instance,
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

    EXQUDENS_VULKAN_INLINE Instance::Builder Instance::builder() {
        return {};
    }

    EXQUDENS_VULKAN_INLINE Instance::Builder& Instance::Builder::setApplicationInfo(const VULKAN_HPP_NAMESPACE::ApplicationInfo& value) {
        resultObject.applicationInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Instance::Builder& Instance::Builder::setEnabledExtensionNames(const std::vector<const char*>& value) {
        resultObject.enabledExtensionNames = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Instance::Builder& Instance::Builder::setEnabledLayerNames(const std::vector<const char*>& value) {
        resultObject.enabledLayerNames = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Instance::Builder& Instance::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::InstanceCreateInfo& value) {
        resultObject.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Instance& Instance::Builder::build(
        Instance& instance,
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

            instance.applicationInfo = resultObject.applicationInfo;
            instance.enabledExtensionNames = resultObject.enabledExtensionNames;
            instance.enabledLayerNames = resultObject.enabledLayerNames;
            instance.createInfo = resultObject.createInfo;
            instance.createInfo.pApplicationInfo = &instance.applicationInfo;
            instance.createInfo.ppEnabledExtensionNames = instance.enabledExtensionNames.data();
            instance.createInfo.enabledExtensionCount = static_cast<uint32_t>(instance.enabledExtensionNames.size());
            instance.createInfo.ppEnabledLayerNames = instance.enabledLayerNames.data();
            instance.createInfo.enabledLayerCount = static_cast<uint32_t>(instance.enabledLayerNames.size());
            instance.target = context.createInstance(instance.createInfo);

            return instance;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
