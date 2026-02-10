#pragma once

#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Semaphore {

        class Builder;

        std::optional<VULKAN_HPP_NAMESPACE::SemaphoreCreateInfo> createInfo = {};
        VULKAN_HPP_NAMESPACE::raii::Semaphore target = nullptr;

        static Builder builder(Semaphore& object);

    };

    class EXQUDENS_VULKAN_EXPORT Semaphore::Builder {

        private:

            Semaphore& object;

        public:

            explicit Builder(Semaphore& object);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::SemaphoreCreateInfo& value);

            Semaphore& build(
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

    EXQUDENS_VULKAN_INLINE Semaphore::Builder Semaphore::builder(Semaphore& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE Semaphore::Builder::Builder(Semaphore& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE Semaphore::Builder& Semaphore::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::SemaphoreCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Semaphore& Semaphore::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.createInfo.has_value()) {
                object.createInfo = VULKAN_HPP_NAMESPACE::SemaphoreCreateInfo();
            }

            object.target = device.createSemaphore(object.createInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
