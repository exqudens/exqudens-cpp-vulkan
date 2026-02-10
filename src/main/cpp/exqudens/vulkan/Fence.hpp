#pragma once

#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Fence {

        class Builder;

        std::optional<VULKAN_HPP_NAMESPACE::FenceCreateInfo> createInfo = {};
        VULKAN_HPP_NAMESPACE::raii::Fence target = nullptr;

        static Builder builder(Fence& object);

    };

    class EXQUDENS_VULKAN_EXPORT Fence::Builder {

        private:

            Fence& object;

        public:

            explicit Builder(Fence& object);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::FenceCreateInfo& value);

            Fence& build(
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

    EXQUDENS_VULKAN_INLINE Fence::Builder Fence::builder(Fence& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE Fence::Builder::Builder(Fence& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE Fence::Builder& Fence::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::FenceCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Fence& Fence::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.createInfo.has_value()) {
                object.createInfo = VULKAN_HPP_NAMESPACE::FenceCreateInfo();
            }

            object.target = device.createFence(object.createInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
