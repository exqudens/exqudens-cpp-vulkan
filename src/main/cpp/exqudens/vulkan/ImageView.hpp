#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <filesystem>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct ImageView {

        class Builder;

        VULKAN_HPP_NAMESPACE::ImageViewCreateInfo createInfo;
        VULKAN_HPP_NAMESPACE::raii::ImageView target = nullptr;

        static Builder builder();

    };

    class ImageView::Builder {

        private:

            ImageView resultObject = {};

        public:

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::ImageViewCreateInfo& value);

            ImageView& build(
                ImageView& imageView,
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

            std::vector<ImageView>& build(
                std::vector<ImageView>& imageViews,
                VULKAN_HPP_NAMESPACE::raii::Device& device,
                const std::vector<VULKAN_HPP_NAMESPACE::Image>& images
            );

    };

    // implementation ---

    inline ImageView::Builder ImageView::builder() {
        return {};
    }

    inline ImageView::Builder& ImageView::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::ImageViewCreateInfo& value) {
        resultObject.createInfo = value;
        return *this;
    }

    inline ImageView& ImageView::Builder::build(
        ImageView& imageView,
        vk::raii::Device& device
    ) {
        try {
            imageView.createInfo = resultObject.createInfo;
            imageView.target = device.createImageView(resultObject.createInfo);

            return imageView;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    inline std::vector<ImageView>& ImageView::Builder::build(
        std::vector<ImageView>& imageViews,
        vk::raii::Device& device,
        const std::vector<vk::Image>& images
    ) {
        try {
            imageViews.clear();
            imageViews.resize(images.size());

            for (size_t i = 0; i < images.size(); i++) {
                imageViews.at(i).createInfo = resultObject.createInfo;
                imageViews.at(i).createInfo.image = images.at(i);

                builder()
                .setCreateInfo(imageViews.at(i).createInfo)
                .build(
                    imageViews.at(i),
                    device
                );
            }

            return imageViews;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
