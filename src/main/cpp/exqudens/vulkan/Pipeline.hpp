#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Pipeline {

        class Builder;

        std::vector<VULKAN_HPP_NAMESPACE::PipelineShaderStageCreateInfo> shaderStageCreateInfos = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineVertexInputStateCreateInfo> vertexInputStateCreateInfo = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineInputAssemblyStateCreateInfo> inputAssemblyStateCreateInfo = {};
        std::vector<VULKAN_HPP_NAMESPACE::Viewport> viewports = {};
        std::vector<VULKAN_HPP_NAMESPACE::Rect2D> scissors = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineViewportStateCreateInfo> viewportStateCreateInfo = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineRasterizationStateCreateInfo> rasterizationStateCreateInfo = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineMultisampleStateCreateInfo> multisampleStateCreateInfo = {};
        std::vector<VULKAN_HPP_NAMESPACE::PipelineColorBlendAttachmentState> colorBlendAttachmentStates = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineColorBlendStateCreateInfo> colorBlendStateCreateInfo = {};
        std::vector<VULKAN_HPP_NAMESPACE::DynamicState> dynamicStates = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineDynamicStateCreateInfo> dynamicStateCreateInfo = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineDepthStencilStateCreateInfo> depthStencilStateCreateInfo = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineTessellationStateCreateInfo> tessellationStateCreateInfo = {};
        std::optional<VULKAN_HPP_NAMESPACE::PipelineRenderingCreateInfo> renderingCreateInfo = {};
        std::optional<VULKAN_HPP_NAMESPACE::GraphicsPipelineCreateInfo> graphicsCreateInfo = {};
        VULKAN_HPP_NAMESPACE::raii::Pipeline target = nullptr;

        static Builder builder(Pipeline& object);

    };

    class EXQUDENS_VULKAN_EXPORT Pipeline::Builder {

        private:

            Pipeline& object;

        public:

            explicit Builder(Pipeline& object);

            Builder& setShaderStageCreateInfos(const std::vector<VULKAN_HPP_NAMESPACE::PipelineShaderStageCreateInfo>& value);

            Builder& addShaderStageCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineShaderStageCreateInfo& value);

            Builder& setVertexInputStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineVertexInputStateCreateInfo& value);

            Builder& setInputAssemblyStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineInputAssemblyStateCreateInfo& value);

            Builder& setViewports(const std::vector<VULKAN_HPP_NAMESPACE::Viewport>& value);

            Builder& addViewport(const VULKAN_HPP_NAMESPACE::Viewport& value);

            Builder& setScissors(const std::vector<VULKAN_HPP_NAMESPACE::Rect2D>& value);

            Builder& addScissor(const VULKAN_HPP_NAMESPACE::Rect2D& value);

            Builder& setViewportStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineViewportStateCreateInfo& value);

            Builder& setRasterizationStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineRasterizationStateCreateInfo& value);

            Builder& setMultisampleStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineMultisampleStateCreateInfo& value);

            Builder& setColorBlendAttachmentStates(const std::vector<VULKAN_HPP_NAMESPACE::PipelineColorBlendAttachmentState>& value);

            Builder& addColorBlendAttachmentState(const VULKAN_HPP_NAMESPACE::PipelineColorBlendAttachmentState& value);

            Builder& setColorBlendStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineColorBlendStateCreateInfo& value);

            Builder& setDynamicStates(const std::vector<VULKAN_HPP_NAMESPACE::DynamicState>& value);

            Builder& addDynamicState(const VULKAN_HPP_NAMESPACE::DynamicState& value);

            Builder& setDynamicStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineDynamicStateCreateInfo& value);

            Builder& setDepthStencilStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineDepthStencilStateCreateInfo& value);

            Builder& setTessellationStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineTessellationStateCreateInfo& value);

            Builder& setRenderingCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineRenderingCreateInfo& value);

            Builder& setGraphicsCreateInfo(const VULKAN_HPP_NAMESPACE::GraphicsPipelineCreateInfo& value);

            Pipeline& build(
                VULKAN_HPP_NAMESPACE::raii::Device& device,
                VULKAN_HPP_NAMESPACE::raii::PipelineCache& cache
            );

            Pipeline& build(
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };
}

// implementation ---

#include <cstdint>
#include <string>
#include <filesystem>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE Pipeline::Builder Pipeline::builder(Pipeline& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder::Builder(Pipeline& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setShaderStageCreateInfos(const std::vector<VULKAN_HPP_NAMESPACE::PipelineShaderStageCreateInfo>& value) {
        object.shaderStageCreateInfos = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addShaderStageCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineShaderStageCreateInfo& value) {
        object.shaderStageCreateInfos.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setVertexInputStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineVertexInputStateCreateInfo& value) {
        object.vertexInputStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setInputAssemblyStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineInputAssemblyStateCreateInfo& value) {
        object.inputAssemblyStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setViewports(const std::vector<VULKAN_HPP_NAMESPACE::Viewport>& value) {
        object.viewports = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addViewport(const VULKAN_HPP_NAMESPACE::Viewport& value) {
        object.viewports.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setScissors(const std::vector<VULKAN_HPP_NAMESPACE::Rect2D>& value) {
        object.scissors = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addScissor(const vk::Rect2D& value) {
        object.scissors.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setViewportStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineViewportStateCreateInfo& value) {
        object.viewportStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setRasterizationStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineRasterizationStateCreateInfo& value) {
        object.rasterizationStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setMultisampleStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineMultisampleStateCreateInfo& value) {
        object.multisampleStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setColorBlendAttachmentStates(const std::vector<VULKAN_HPP_NAMESPACE::PipelineColorBlendAttachmentState>& value) {
        object.colorBlendAttachmentStates = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addColorBlendAttachmentState(const VULKAN_HPP_NAMESPACE::PipelineColorBlendAttachmentState& value) {
        object.colorBlendAttachmentStates.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setColorBlendStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineColorBlendStateCreateInfo& value) {
        object.colorBlendStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setDynamicStates(const std::vector<VULKAN_HPP_NAMESPACE::DynamicState>& value) {
        object.dynamicStates = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addDynamicState(const VULKAN_HPP_NAMESPACE::DynamicState& value) {
        object.dynamicStates.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setDynamicStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineDynamicStateCreateInfo& value) {
        object.dynamicStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setDepthStencilStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineDepthStencilStateCreateInfo& value) {
        object.depthStencilStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setTessellationStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineTessellationStateCreateInfo& value) {
        object.tessellationStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setRenderingCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineRenderingCreateInfo& value) {
        object.renderingCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setGraphicsCreateInfo(const VULKAN_HPP_NAMESPACE::GraphicsPipelineCreateInfo& value) {
        object.graphicsCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline& Pipeline::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device,
        VULKAN_HPP_NAMESPACE::raii::PipelineCache& cache
    ) {
        try {
            if (!object.viewports.empty() || !object.scissors.empty()) {
                if (!object.viewportStateCreateInfo.has_value()) {
                    VULKAN_HPP_NAMESPACE::PipelineViewportStateCreateInfo v;
                    object.viewportStateCreateInfo = v;
                }

                if (!object.viewports.empty()) {
                    object.viewportStateCreateInfo.value().viewportCount = static_cast<uint32_t>(object.viewports.size());
                    object.viewportStateCreateInfo.value().pViewports = object.viewports.data();
                }

                if (!object.scissors.empty()) {
                    object.viewportStateCreateInfo.value().scissorCount = static_cast<uint32_t>(object.scissors.size());
                    object.viewportStateCreateInfo.value().pScissors = object.scissors.data();
                }
            }

            if (!object.colorBlendAttachmentStates.empty()) {
                if (!object.colorBlendStateCreateInfo.has_value()) {
                    VULKAN_HPP_NAMESPACE::PipelineColorBlendStateCreateInfo v;
                    object.colorBlendStateCreateInfo = v;
                }
                object.colorBlendStateCreateInfo.value().attachmentCount = static_cast<uint32_t>(object.colorBlendAttachmentStates.size());
                object.colorBlendStateCreateInfo.value().pAttachments = object.colorBlendAttachmentStates.data();
            }

            if (!object.dynamicStates.empty()) {
                if (!object.dynamicStateCreateInfo.has_value()) {
                    VULKAN_HPP_NAMESPACE::PipelineDynamicStateCreateInfo v;
                    object.dynamicStateCreateInfo = v;
                }
                object.dynamicStateCreateInfo.value().dynamicStateCount = static_cast<uint32_t>(object.dynamicStates.size());
                object.dynamicStateCreateInfo.value().pDynamicStates = object.dynamicStates.data();
            }

            if (object.graphicsCreateInfo.has_value()) {
                if (!object.shaderStageCreateInfos.empty()) {
                    object.graphicsCreateInfo.value().stageCount = static_cast<uint32_t>(object.shaderStageCreateInfos.size());
                    object.graphicsCreateInfo.value().pStages = object.shaderStageCreateInfos.data();
                }

                if (object.vertexInputStateCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pVertexInputState = &object.vertexInputStateCreateInfo.value();
                }

                if (object.inputAssemblyStateCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pInputAssemblyState = &object.inputAssemblyStateCreateInfo.value();
                }

                if (object.viewportStateCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pViewportState = &object.viewportStateCreateInfo.value();
                }

                if (object.rasterizationStateCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pRasterizationState = &object.rasterizationStateCreateInfo.value();
                }

                if (object.multisampleStateCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pMultisampleState = &object.multisampleStateCreateInfo.value();
                }

                if (object.colorBlendStateCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pColorBlendState = &object.colorBlendStateCreateInfo.value();
                }

                if (object.dynamicStateCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pDynamicState = &object.dynamicStateCreateInfo.value();
                }

                if (object.depthStencilStateCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pDepthStencilState = &object.depthStencilStateCreateInfo.value();
                }

                if (object.tessellationStateCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pTessellationState = &object.tessellationStateCreateInfo.value();
                }

                if (object.renderingCreateInfo.has_value()) {
                    object.graphicsCreateInfo.value().pNext = &object.renderingCreateInfo.value();
                }

                object.target = device.createGraphicsPipeline(cache, object.graphicsCreateInfo.value());
            }

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE Pipeline& Pipeline::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            VULKAN_HPP_NAMESPACE::raii::PipelineCache cache = nullptr;
            return build(device, cache);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
