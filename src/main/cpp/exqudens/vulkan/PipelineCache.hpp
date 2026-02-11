#pragma once

#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT PipelineCache {

        class Builder;

        std::optional<VULKAN_HPP_NAMESPACE::PipelineCacheCreateInfo> createInfo = {};
        VULKAN_HPP_NAMESPACE::raii::PipelineCache target = nullptr;

        static Builder builder(PipelineCache& object);

        void clear();

        void clearAndRelease();
    };

    class EXQUDENS_VULKAN_EXPORT PipelineCache::Builder {

        private:

            PipelineCache& object;

        public:

            explicit Builder(PipelineCache& object);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineCacheCreateInfo& value);

            PipelineCache& build(
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

    EXQUDENS_VULKAN_INLINE PipelineCache::Builder PipelineCache::builder(PipelineCache& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE void PipelineCache::clear() {
        try {
            createInfo.reset();
            target.clear();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void PipelineCache::clearAndRelease() {
        try {
            clear();
            target.release();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE PipelineCache::Builder::Builder(PipelineCache& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE PipelineCache::Builder& PipelineCache::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineCacheCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE PipelineCache& PipelineCache::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.createInfo.has_value()) {
                object.createInfo = VULKAN_HPP_NAMESPACE::PipelineCacheCreateInfo();
            }

            object.target = device.createPipelineCache(object.createInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
