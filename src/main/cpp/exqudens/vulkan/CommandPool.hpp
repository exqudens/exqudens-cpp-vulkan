#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT CommandPool {

        class Builder;

        VULKAN_HPP_NAMESPACE::CommandPoolCreateInfo createInfo;
        VULKAN_HPP_NAMESPACE::raii::CommandPool target = nullptr;

        static Builder builder(CommandPool& object);

    };

    class EXQUDENS_VULKAN_EXPORT CommandPool::Builder {

        private:

            CommandPool& object;

        public:

            explicit Builder(CommandPool& object);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::CommandPoolCreateInfo& value);

            CommandPool& build(
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };

}

// implementation ---

#include <string>
#include <filesystem>
#include <stdexcept>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE CommandPool::Builder CommandPool::builder(CommandPool& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE CommandPool::Builder::Builder(CommandPool& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE CommandPool::Builder& CommandPool::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::CommandPoolCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE CommandPool& CommandPool::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            object.target = device.createCommandPool(object.createInfo);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
