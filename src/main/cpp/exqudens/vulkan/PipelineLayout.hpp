#pragma once

#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT PipelineLayout {

        class Builder;

        std::optional<VULKAN_HPP_NAMESPACE::PipelineLayoutCreateInfo> createInfo = {};
        VULKAN_HPP_NAMESPACE::raii::PipelineLayout target = nullptr;

        static Builder builder(PipelineLayout& object);

        void clear();

        void clearAndRelease();

    };

    class EXQUDENS_VULKAN_EXPORT PipelineLayout::Builder {

        private:

            PipelineLayout& object;

        public:

            explicit Builder(PipelineLayout& object);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineLayoutCreateInfo& value);

            PipelineLayout& build(
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

    EXQUDENS_VULKAN_INLINE PipelineLayout::Builder PipelineLayout::builder(PipelineLayout& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE void PipelineLayout::clear() {
        try {
            createInfo.reset();
            target.clear();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void PipelineLayout::clearAndRelease() {
        try {
            clear();
            target.release();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE PipelineLayout::Builder::Builder(PipelineLayout& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE PipelineLayout::Builder& PipelineLayout::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineLayoutCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE PipelineLayout& PipelineLayout::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.createInfo.has_value()) {
                object.createInfo = VULKAN_HPP_NAMESPACE::PipelineLayoutCreateInfo();
            }

            object.target = device.createPipelineLayout(object.createInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
