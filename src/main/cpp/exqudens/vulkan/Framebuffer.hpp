#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Framebuffer {

        class Builder;

        std::vector<VULKAN_HPP_NAMESPACE::ImageView> attachments = {};
        std::optional<VULKAN_HPP_NAMESPACE::FramebufferCreateInfo> createInfo = {};
        VULKAN_HPP_NAMESPACE::raii::Framebuffer target = nullptr;

        static Builder builder(Framebuffer& object);

        void clear();

        void clearAndRelease();

    };

    class EXQUDENS_VULKAN_EXPORT Framebuffer::Builder {

    private:

        Framebuffer& object;

    public:

        explicit Builder(Framebuffer& object);

        Builder& setAttachments(const std::vector<VULKAN_HPP_NAMESPACE::ImageView>& value);

        Builder& addAttachment(const VULKAN_HPP_NAMESPACE::ImageView& value);

        Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::FramebufferCreateInfo& value);

        Framebuffer& build(
            VULKAN_HPP_NAMESPACE::raii::Device& device
        );

    };
}

// implementation ---

#include <cstdint>
#include <string>
#include <filesystem>
#include <stdexcept>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE Framebuffer::Builder Framebuffer::builder(Framebuffer& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE void Framebuffer::clear() {
        try {
            attachments.clear();
            createInfo.reset();
            target.clear();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void Framebuffer::clearAndRelease() {
        try {
            clear();
            target.release();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE Framebuffer::Builder::Builder(Framebuffer& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE Framebuffer::Builder& Framebuffer::Builder::setAttachments(const std::vector<VULKAN_HPP_NAMESPACE::ImageView>& value) {
        object.attachments = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Framebuffer::Builder& Framebuffer::Builder::addAttachment(const VULKAN_HPP_NAMESPACE::ImageView& value) {
        object.attachments.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Framebuffer::Builder& Framebuffer::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::FramebufferCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Framebuffer& Framebuffer::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.createInfo.has_value()) {
                object.createInfo = VULKAN_HPP_NAMESPACE::FramebufferCreateInfo();
            }

            object.createInfo.value().attachmentCount = static_cast<uint32_t>(object.attachments.size());
            object.createInfo.value().pAttachments = object.attachments.empty() ? nullptr : object.attachments.data();
            object.target = device.createFramebuffer(object.createInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
