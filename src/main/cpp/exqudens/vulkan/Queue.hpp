#pragma once

#include <cstdint>
#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Queue {

        class Builder;

        std::optional<uint32_t> familyIndex = {};
        std::optional<uint32_t> index = {};
        VULKAN_HPP_NAMESPACE::raii::Queue target = nullptr;

        static Builder builder(Queue& object);

    };

    class EXQUDENS_VULKAN_EXPORT Queue::Builder {

        private:

            Queue& object;

        public:

            explicit Builder(Queue& object);

            Builder& setFamilyIndex(const std::optional<uint32_t>& value);

            Builder& setIndex(const std::optional<uint32_t>& value);

            Queue& build(
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };
}

// implementation ---

#include <string>
#include <filesystem>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE Queue::Builder Queue::builder(Queue& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE Queue::Builder::Builder(Queue& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE Queue::Builder& Queue::Builder::setFamilyIndex(const std::optional<uint32_t>& value) {
        object.familyIndex = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Queue::Builder& Queue::Builder::setIndex(const std::optional<uint32_t>& value) {
        object.index = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Queue& Queue::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.familyIndex.has_value()) {
                throw std::runtime_error(CALL_INFO + ": family index not set");
            }

            if (!object.index.has_value()) {
                object.index = 0;
            }

            object.target = device.getQueue(object.familyIndex.value(), object.index.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
