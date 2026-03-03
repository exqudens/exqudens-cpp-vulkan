#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT DescriptorSetLayout {

        class Builder;

        std::vector<VULKAN_HPP_NAMESPACE::DescriptorSetLayoutBinding> bindings = {};
        std::optional<VULKAN_HPP_NAMESPACE::DescriptorSetLayoutCreateInfo> createInfo = {};
        VULKAN_HPP_NAMESPACE::raii::DescriptorSetLayout target = nullptr;

        static Builder builder(DescriptorSetLayout& object);

        void clear();

        void clearAndRelease();

    };

    class EXQUDENS_VULKAN_EXPORT DescriptorSetLayout::Builder {

        private:

            DescriptorSetLayout& object;

        public:

            explicit Builder(DescriptorSetLayout& object);

            Builder& setBindings(const std::vector<VULKAN_HPP_NAMESPACE::DescriptorSetLayoutBinding>& value);

            Builder& addBinding(const VULKAN_HPP_NAMESPACE::DescriptorSetLayoutBinding& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::DescriptorSetLayoutCreateInfo& value);

            DescriptorSetLayout& build(
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

    EXQUDENS_VULKAN_INLINE DescriptorSetLayout::Builder DescriptorSetLayout::builder(DescriptorSetLayout& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE void DescriptorSetLayout::clear() {
        try {
            bindings.clear();
            createInfo.reset();
            target.clear();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE void DescriptorSetLayout::clearAndRelease() {
        try {
            clear();
            target.release();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE DescriptorSetLayout::Builder::Builder(DescriptorSetLayout& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::setBindings(const std::vector<VULKAN_HPP_NAMESPACE::DescriptorSetLayoutBinding>& value) {
        object.bindings = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding(const VULKAN_HPP_NAMESPACE::DescriptorSetLayoutBinding& value) {
        object.bindings.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::DescriptorSetLayoutCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE DescriptorSetLayout& DescriptorSetLayout::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!object.createInfo.has_value()) {
                object.createInfo = VULKAN_HPP_NAMESPACE::DescriptorSetLayoutCreateInfo();
            }

            object.createInfo.value().bindingCount = static_cast<uint32_t>(object.bindings.size());
            object.createInfo.value().pBindings = object.bindings.empty() ? nullptr : object.bindings.data();

            object.target = device.createDescriptorSetLayout(object.createInfo.value());

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
