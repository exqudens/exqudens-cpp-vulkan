#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <filesystem>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct DebugUtilsMessenger {

        class Builder;

        VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT createInfo;

        static Builder builder();

    };

    class DebugUtilsMessenger::Builder {

        private:

            DebugUtilsMessenger resultObject = {};

        public:

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT& value);

            DebugUtilsMessenger build(
                VULKAN_HPP_NAMESPACE::raii::DebugUtilsMessengerEXT& debugUtilsMessenger,
                VULKAN_HPP_NAMESPACE::raii::Instance& instance
            );

    };

    // implementation ---

    inline DebugUtilsMessenger::Builder DebugUtilsMessenger::builder() {
        return {};
    }

    inline DebugUtilsMessenger::Builder& DebugUtilsMessenger::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT& value) {
        resultObject.createInfo = value;
        return *this;
    }

    inline DebugUtilsMessenger DebugUtilsMessenger::Builder::build(VULKAN_HPP_NAMESPACE::raii::DebugUtilsMessengerEXT& debugUtilsMessenger, VULKAN_HPP_NAMESPACE::raii::Instance& instance) {
        try {
            DebugUtilsMessenger result = {};

            result.createInfo = resultObject.createInfo;
            debugUtilsMessenger = instance.createDebugUtilsMessengerEXT(result.createInfo);

            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
