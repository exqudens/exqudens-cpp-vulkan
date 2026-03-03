#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT DescriptorPool {

        class Builder;

        std::vector<VULKAN_HPP_NAMESPACE::DescriptorPoolSize> sizes = {};
        std::optional<VULKAN_HPP_NAMESPACE::DescriptorPoolCreateInfo> createInfo = {};
        VULKAN_HPP_NAMESPACE::raii::DescriptorPool target = nullptr;

        static Builder builder(DescriptorPool& object);

        void clear();

        void clearAndRelease();

    };

    class EXQUDENS_VULKAN_EXPORT DescriptorPool::Builder {

        private:

            DescriptorPool& object;

        public:

            explicit Builder(DescriptorPool& object);

            Builder& setSizes(const std::vector<VULKAN_HPP_NAMESPACE::DescriptorPoolSize>& value);

            Builder& addSize(const VULKAN_HPP_NAMESPACE::DescriptorPoolSize& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::DescriptorPoolCreateInfo& value);

            DescriptorPool& build(
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

    EXQUDENS_VULKAN_INLINE DescriptorPool::Builder DescriptorPool::builder(DescriptorPool& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE void DescriptorPool::clear() {
        try {
            sizes.clear();
            createInfo.reset();
            target.clear();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void DescriptorPool::clearAndRelease() {
        try {
            clear();
            target.release();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE DescriptorPool::Builder::Builder(DescriptorPool& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE DescriptorPool::Builder& DescriptorPool::Builder::setSizes(const std::vector<VULKAN_HPP_NAMESPACE::DescriptorPoolSize>& value) {
        object.sizes = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DescriptorPool::Builder& DescriptorPool::Builder::addSize(const VULKAN_HPP_NAMESPACE::DescriptorPoolSize& value) {
        object.sizes.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DescriptorPool::Builder& DescriptorPool::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::DescriptorPoolCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DescriptorPool& DescriptorPool::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.createInfo.has_value()) {
                object.createInfo = VULKAN_HPP_NAMESPACE::DescriptorPoolCreateInfo();
            }

            object.createInfo.value().poolSizeCount = static_cast<uint32_t>(object.sizes.size());
            object.createInfo.value().pPoolSizes = object.sizes.empty() ? nullptr : object.sizes.data();

            object.target = device.createDescriptorPool(object.createInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
