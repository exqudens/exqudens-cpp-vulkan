#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT PipelineLayout {

        class Builder;

        VULKAN_HPP_NAMESPACE::PipelineLayoutCreateInfo createInfo;
        VULKAN_HPP_NAMESPACE::raii::PipelineLayout target = nullptr;

        static Builder builder();

    };

    class EXQUDENS_VULKAN_EXPORT PipelineLayout::Builder {

        private:

            PipelineLayout resultObject = {};

        public:

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineLayoutCreateInfo& value);

            PipelineLayout& build(
                PipelineLayout& pipelineLayout,
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };
}

// implementation ---

#include <string>
#include <filesystem>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE PipelineLayout::Builder PipelineLayout::builder() {
        return {};
    }

    EXQUDENS_VULKAN_INLINE PipelineLayout::Builder& PipelineLayout::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineLayoutCreateInfo& value) {
        resultObject.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE PipelineLayout& PipelineLayout::Builder::build(
        PipelineLayout& pipelineLayout,
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            pipelineLayout.createInfo = resultObject.createInfo;
            pipelineLayout.target = device.createPipelineLayout(pipelineLayout.createInfo);

            return pipelineLayout;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
