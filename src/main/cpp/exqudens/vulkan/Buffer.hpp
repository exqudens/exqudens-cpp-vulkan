#pragma once

#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Buffer {

        class Builder;

        std::optional<VULKAN_HPP_NAMESPACE::BufferCreateInfo> createInfo = {};
        VULKAN_HPP_NAMESPACE::raii::Buffer target = nullptr;

        static Builder builder(Buffer& object);

        void clear();

        void clearAndRelease();

    };

    class EXQUDENS_VULKAN_EXPORT Buffer::Builder {

    private:

        Buffer& object;

    public:

        explicit Builder(Buffer& object);

        Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::BufferCreateInfo& value);

        Buffer& build(
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

    EXQUDENS_VULKAN_INLINE Buffer::Builder Buffer::builder(Buffer& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE void Buffer::clear() {
        try {
            createInfo.reset();
            target.clear();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void Buffer::clearAndRelease() {
        try {
            clear();
            target.release();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE Buffer::Builder::Builder(Buffer& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE Buffer::Builder& Buffer::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::BufferCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Buffer& Buffer::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.createInfo.has_value()) {
                object.createInfo = VULKAN_HPP_NAMESPACE::BufferCreateInfo();
            }

            object.target = device.createBuffer(object.createInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
