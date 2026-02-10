#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT ImageView {

        class Builder;

        VULKAN_HPP_NAMESPACE::ImageViewCreateInfo createInfo;
        VULKAN_HPP_NAMESPACE::raii::ImageView target = nullptr;

        static Builder builder(ImageView& object);

    };

    class EXQUDENS_VULKAN_EXPORT ImageView::Builder {

        private:

            ImageView& object;

        public:

            explicit Builder(ImageView& object);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::ImageViewCreateInfo& value);

            ImageView& build(
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

    EXQUDENS_VULKAN_INLINE ImageView::Builder ImageView::builder(ImageView& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE ImageView::Builder::Builder(ImageView& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE ImageView::Builder& ImageView::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::ImageViewCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE ImageView& ImageView::Builder::build(
        vk::raii::Device& device
    ) {
        try {
            object.target = device.createImageView(object.createInfo);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
