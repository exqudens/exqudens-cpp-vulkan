#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT DescriptorSets {

        class Builder;

        std::vector<VULKAN_HPP_NAMESPACE::DescriptorSetLayout> layouts = {};
        std::optional<VULKAN_HPP_NAMESPACE::DescriptorSetAllocateInfo> allocateInfo = {};
        VULKAN_HPP_NAMESPACE::raii::DescriptorSets targets = nullptr;

        static Builder builder(DescriptorSets& object);

        void clear();

        void clearAndRelease();

    };

    class EXQUDENS_VULKAN_EXPORT DescriptorSets::Builder {

        private:

            DescriptorSets& object;

        public:

            explicit Builder(DescriptorSets& object);

            Builder& setLayouts(const std::vector<VULKAN_HPP_NAMESPACE::DescriptorSetLayout>& value);

            Builder& addLayout(const VULKAN_HPP_NAMESPACE::DescriptorSetLayout& value);

            Builder& setAllocateInfo(const VULKAN_HPP_NAMESPACE::DescriptorSetAllocateInfo& value);

            DescriptorSets& build(
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

    EXQUDENS_VULKAN_INLINE DescriptorSets::Builder DescriptorSets::builder(DescriptorSets& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE void DescriptorSets::clear() {
        try {
            layouts.clear();
            allocateInfo.reset();
            targets.clear();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void DescriptorSets::clearAndRelease() {
        try {
            clear();
            targets = nullptr;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE DescriptorSets::Builder::Builder(DescriptorSets& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE DescriptorSets::Builder& DescriptorSets::Builder::setLayouts(const std::vector<VULKAN_HPP_NAMESPACE::DescriptorSetLayout>& value) {
        object.layouts = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DescriptorSets::Builder& DescriptorSets::Builder::addLayout(const VULKAN_HPP_NAMESPACE::DescriptorSetLayout& value) {
        object.layouts.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DescriptorSets::Builder& DescriptorSets::Builder::setAllocateInfo(const VULKAN_HPP_NAMESPACE::DescriptorSetAllocateInfo& value) {
        object.allocateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DescriptorSets& DescriptorSets::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.allocateInfo.has_value()) {
                object.allocateInfo = VULKAN_HPP_NAMESPACE::DescriptorSetAllocateInfo();
            }

            object.allocateInfo.value().descriptorSetCount = static_cast<uint32_t>(object.layouts.size());
            object.allocateInfo.value().pSetLayouts = object.layouts.empty() ? nullptr : object.layouts.data();

            object.targets = VULKAN_HPP_NAMESPACE::raii::DescriptorSets(device, object.allocateInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
