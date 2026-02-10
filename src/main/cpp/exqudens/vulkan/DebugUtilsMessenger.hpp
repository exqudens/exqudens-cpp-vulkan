#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT DebugUtilsMessenger {

        class Builder;

        VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT createInfo;
        VULKAN_HPP_NAMESPACE::raii::DebugUtilsMessengerEXT target = nullptr;

        static Builder builder(DebugUtilsMessenger& object);

    };

    class EXQUDENS_VULKAN_EXPORT DebugUtilsMessenger::Builder {

        private:

            DebugUtilsMessenger& object;

        public:

            explicit Builder(DebugUtilsMessenger& object);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT& value);

            DebugUtilsMessenger& build(
                VULKAN_HPP_NAMESPACE::raii::Instance& instance
            );

    };
}

// implementation ---

#include <string>
#include <filesystem>
#include <stdexcept>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE DebugUtilsMessenger::Builder DebugUtilsMessenger::builder(DebugUtilsMessenger& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE DebugUtilsMessenger::Builder::Builder(DebugUtilsMessenger& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE DebugUtilsMessenger::Builder& DebugUtilsMessenger::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DebugUtilsMessenger& DebugUtilsMessenger::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Instance& instance
    ) {
        try {
            object.target = instance.createDebugUtilsMessengerEXT(object.createInfo);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
