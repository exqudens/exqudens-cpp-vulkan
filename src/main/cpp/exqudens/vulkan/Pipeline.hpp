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

        static Builder builder();

    };

    class EXQUDENS_VULKAN_EXPORT Pipeline::Builder {

        private:

            Pipeline resultObject = {};

        public:

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
                Pipeline& pipeline,
                VULKAN_HPP_NAMESPACE::raii::PipelineLayout& layout,
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

    EXQUDENS_VULKAN_INLINE Pipeline::Builder Pipeline::builder() {
        return {};
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setShaderStageCreateInfos(const std::vector<VULKAN_HPP_NAMESPACE::PipelineShaderStageCreateInfo>& value) {
        resultObject.shaderStageCreateInfos = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addShaderStageCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineShaderStageCreateInfo& value) {
        resultObject.shaderStageCreateInfos.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setVertexInputStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineVertexInputStateCreateInfo& value) {
        resultObject.vertexInputStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setInputAssemblyStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineInputAssemblyStateCreateInfo& value) {
        resultObject.inputAssemblyStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setViewports(const std::vector<VULKAN_HPP_NAMESPACE::Viewport>& value) {
        resultObject.viewports = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addViewport(const VULKAN_HPP_NAMESPACE::Viewport& value) {
        resultObject.viewports.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setScissors(const std::vector<VULKAN_HPP_NAMESPACE::Rect2D>& value) {
        resultObject.scissors = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addScissor(const vk::Rect2D& value) {
        resultObject.scissors.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setViewportStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineViewportStateCreateInfo& value) {
        resultObject.viewportStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setRasterizationStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineRasterizationStateCreateInfo& value) {
        resultObject.rasterizationStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setMultisampleStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineMultisampleStateCreateInfo& value) {
        resultObject.multisampleStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setColorBlendAttachmentStates(const std::vector<VULKAN_HPP_NAMESPACE::PipelineColorBlendAttachmentState>& value) {
        resultObject.colorBlendAttachmentStates = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addColorBlendAttachmentState(const VULKAN_HPP_NAMESPACE::PipelineColorBlendAttachmentState& value) {
        resultObject.colorBlendAttachmentStates.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setColorBlendStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineColorBlendStateCreateInfo& value) {
        resultObject.colorBlendStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setDynamicStates(const std::vector<VULKAN_HPP_NAMESPACE::DynamicState>& value) {
        resultObject.dynamicStates = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::addDynamicState(const VULKAN_HPP_NAMESPACE::DynamicState& value) {
        resultObject.dynamicStates.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setDynamicStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineDynamicStateCreateInfo& value) {
        resultObject.dynamicStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setDepthStencilStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineDepthStencilStateCreateInfo& value) {
        resultObject.depthStencilStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setTessellationStateCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineTessellationStateCreateInfo& value) {
        resultObject.tessellationStateCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setRenderingCreateInfo(const VULKAN_HPP_NAMESPACE::PipelineRenderingCreateInfo& value) {
        resultObject.renderingCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline::Builder& Pipeline::Builder::setGraphicsCreateInfo(const VULKAN_HPP_NAMESPACE::GraphicsPipelineCreateInfo& value) {
        resultObject.graphicsCreateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Pipeline& Pipeline::Builder::build(
        Pipeline& pipeline,
        VULKAN_HPP_NAMESPACE::raii::PipelineLayout& layout,
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            pipeline.shaderStageCreateInfos = resultObject.shaderStageCreateInfos;
            pipeline.vertexInputStateCreateInfo = resultObject.vertexInputStateCreateInfo;
            pipeline.inputAssemblyStateCreateInfo = resultObject.inputAssemblyStateCreateInfo;
            pipeline.viewports = resultObject.viewports;
            pipeline.scissors = resultObject.scissors;
            pipeline.viewportStateCreateInfo = resultObject.viewportStateCreateInfo;
            pipeline.rasterizationStateCreateInfo = resultObject.rasterizationStateCreateInfo;
            pipeline.multisampleStateCreateInfo = resultObject.multisampleStateCreateInfo;
            pipeline.colorBlendAttachmentStates = resultObject.colorBlendAttachmentStates;
            pipeline.colorBlendStateCreateInfo = resultObject.colorBlendStateCreateInfo;
            pipeline.dynamicStates = resultObject.dynamicStates;
            pipeline.dynamicStateCreateInfo = resultObject.dynamicStateCreateInfo;
            pipeline.depthStencilStateCreateInfo = resultObject.depthStencilStateCreateInfo;
            pipeline.tessellationStateCreateInfo = resultObject.tessellationStateCreateInfo;
            pipeline.renderingCreateInfo = resultObject.renderingCreateInfo;
            pipeline.graphicsCreateInfo = resultObject.graphicsCreateInfo;

            if (!pipeline.viewports.empty() || !pipeline.scissors.empty()) {
                if (!pipeline.viewportStateCreateInfo.has_value()) {
                    VULKAN_HPP_NAMESPACE::PipelineViewportStateCreateInfo v;
                    pipeline.viewportStateCreateInfo = v;
                }

                if (!pipeline.viewports.empty()) {
                    pipeline.viewportStateCreateInfo.value().viewportCount = static_cast<uint32_t>(pipeline.viewports.size());
                    pipeline.viewportStateCreateInfo.value().pViewports = pipeline.viewports.data();
                }

                if (!pipeline.scissors.empty()) {
                    pipeline.viewportStateCreateInfo.value().scissorCount = static_cast<uint32_t>(pipeline.scissors.size());
                    pipeline.viewportStateCreateInfo.value().pScissors = pipeline.scissors.data();
                }
            }

            if (!pipeline.colorBlendAttachmentStates.empty()) {
                if (!pipeline.colorBlendStateCreateInfo.has_value()) {
                    VULKAN_HPP_NAMESPACE::PipelineColorBlendStateCreateInfo v;
                    pipeline.colorBlendStateCreateInfo = v;
                }
                pipeline.colorBlendStateCreateInfo.value().attachmentCount = static_cast<uint32_t>(pipeline.colorBlendAttachmentStates.size());
                pipeline.colorBlendStateCreateInfo.value().pAttachments = pipeline.colorBlendAttachmentStates.data();
            }

            if (!pipeline.dynamicStates.empty()) {
                if (!pipeline.dynamicStateCreateInfo.has_value()) {
                    VULKAN_HPP_NAMESPACE::PipelineDynamicStateCreateInfo v;
                    pipeline.dynamicStateCreateInfo = v;
                }
                pipeline.dynamicStateCreateInfo.value().dynamicStateCount = static_cast<uint32_t>(pipeline.dynamicStates.size());
                pipeline.dynamicStateCreateInfo.value().pDynamicStates = pipeline.dynamicStates.data();
            }

            if (pipeline.graphicsCreateInfo.has_value()) {
                if (!pipeline.shaderStageCreateInfos.empty()) {
                    pipeline.graphicsCreateInfo.value().stageCount = static_cast<uint32_t>(pipeline.shaderStageCreateInfos.size());
                    pipeline.graphicsCreateInfo.value().pStages = pipeline.shaderStageCreateInfos.data();
                }

                if (pipeline.vertexInputStateCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pVertexInputState = &pipeline.vertexInputStateCreateInfo.value();
                }

                if (pipeline.inputAssemblyStateCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pInputAssemblyState = &pipeline.inputAssemblyStateCreateInfo.value();
                }

                if (pipeline.viewportStateCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pViewportState = &pipeline.viewportStateCreateInfo.value();
                }

                if (pipeline.rasterizationStateCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pRasterizationState = &pipeline.rasterizationStateCreateInfo.value();
                }

                if (pipeline.multisampleStateCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pMultisampleState = &pipeline.multisampleStateCreateInfo.value();
                }

                if (pipeline.colorBlendStateCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pColorBlendState = &pipeline.colorBlendStateCreateInfo.value();
                }

                if (pipeline.dynamicStateCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pDynamicState = &pipeline.dynamicStateCreateInfo.value();
                }

                if (pipeline.depthStencilStateCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pDepthStencilState = &pipeline.depthStencilStateCreateInfo.value();
                }

                if (pipeline.tessellationStateCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pTessellationState = &pipeline.tessellationStateCreateInfo.value();
                }

                if (pipeline.renderingCreateInfo.has_value()) {
                    pipeline.graphicsCreateInfo.value().pNext = &pipeline.renderingCreateInfo.value();
                }

                pipeline.graphicsCreateInfo.value().layout = layout;
                pipeline.graphicsCreateInfo.value().renderPass = nullptr;
                pipeline.target = device.createGraphicsPipeline(nullptr, pipeline.graphicsCreateInfo.value());
            }

            return pipeline;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
