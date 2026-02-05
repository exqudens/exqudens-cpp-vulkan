#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT DebugUtilsMessenger {

        class Builder;

        VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT createInfo;
        VULKAN_HPP_NAMESPACE::raii::DebugUtilsMessengerEXT target = nullptr;

        static Builder builder();

    };

    class EXQUDENS_VULKAN_EXPORT DebugUtilsMessenger::Builder {

        private:

            DebugUtilsMessenger resultObject = {};

        public:

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT& value);

            DebugUtilsMessenger& build(
                DebugUtilsMessenger& debugUtilsMessenger,
                VULKAN_HPP_NAMESPACE::raii::Instance& instance
            );

    };
}

// implementation ---

#include <string>
#include <filesystem>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE DebugUtilsMessenger::Builder DebugUtilsMessenger::builder() {
        return {};
    }

    EXQUDENS_VULKAN_INLINE DebugUtilsMessenger::Builder& DebugUtilsMessenger::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT& value) {
        resultObject.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DebugUtilsMessenger& DebugUtilsMessenger::Builder::build(
        DebugUtilsMessenger& debugUtilsMessenger,
        VULKAN_HPP_NAMESPACE::raii::Instance& instance
    ) {
        try {
            debugUtilsMessenger.createInfo = resultObject.createInfo;
            debugUtilsMessenger.target = instance.createDebugUtilsMessengerEXT(debugUtilsMessenger.createInfo);

            return debugUtilsMessenger;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
