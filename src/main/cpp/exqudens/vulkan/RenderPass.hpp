#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT RenderPass {

        class Builder;

        std::vector<VULKAN_HPP_NAMESPACE::AttachmentDescription> attachmentDescriptions = {};
        std::vector<std::vector<VULKAN_HPP_NAMESPACE::AttachmentReference>> subpassDescriptionAttachmentReferences = {};
        std::vector<VULKAN_HPP_NAMESPACE::SubpassDescription> subpassDescriptions = {};
        std::vector<VULKAN_HPP_NAMESPACE::SubpassDependency> subpassDependencies = {};
        VULKAN_HPP_NAMESPACE::RenderPassCreateInfo createInfo;
        VULKAN_HPP_NAMESPACE::raii::RenderPass target = nullptr;

        static Builder builder(RenderPass& object);

    };

    class EXQUDENS_VULKAN_EXPORT RenderPass::Builder {

        private:

            RenderPass& object;

        public:

            explicit Builder(RenderPass& object);

            Builder& setAttachmentDescriptions(const std::vector<VULKAN_HPP_NAMESPACE::AttachmentDescription>& value);

            Builder& addAttachmentDescription(const VULKAN_HPP_NAMESPACE::AttachmentDescription& value);

            Builder& setSubpassDescriptionAttachmentReferences(const std::vector<std::vector<VULKAN_HPP_NAMESPACE::AttachmentReference>>& value);

            Builder& addSubpassDescriptionAttachmentReferences(const std::vector<VULKAN_HPP_NAMESPACE::AttachmentReference>& value);

            Builder& setSubpassDescriptions(const std::vector<VULKAN_HPP_NAMESPACE::SubpassDescription>& value);

            Builder& addSubpassDescription(const VULKAN_HPP_NAMESPACE::SubpassDescription& value);

            Builder& setSubpassDependencies(const std::vector<VULKAN_HPP_NAMESPACE::SubpassDependency>& value);

            Builder& addSubpassDependency(const VULKAN_HPP_NAMESPACE::SubpassDependency& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::RenderPassCreateInfo& value);

            RenderPass& build(
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };

}

// implementation ---

#include <cstddef>
#include <cstdint>
#include <string>
#include <filesystem>
#include <stdexcept>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE RenderPass::Builder RenderPass::builder(RenderPass& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder::Builder(RenderPass& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder& RenderPass::Builder::setAttachmentDescriptions(const std::vector<VULKAN_HPP_NAMESPACE::AttachmentDescription>& value) {
        object.attachmentDescriptions = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder& RenderPass::Builder::addAttachmentDescription(const VULKAN_HPP_NAMESPACE::AttachmentDescription& value) {
        object.attachmentDescriptions.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder& RenderPass::Builder::setSubpassDescriptionAttachmentReferences(const std::vector<std::vector<VULKAN_HPP_NAMESPACE::AttachmentReference>>& value) {
        object.subpassDescriptionAttachmentReferences = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder& RenderPass::Builder::addSubpassDescriptionAttachmentReferences(const std::vector<VULKAN_HPP_NAMESPACE::AttachmentReference>& value) {
        object.subpassDescriptionAttachmentReferences.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder& RenderPass::Builder::setSubpassDescriptions(const std::vector<VULKAN_HPP_NAMESPACE::SubpassDescription>& value) {
        object.subpassDescriptions = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder& RenderPass::Builder::addSubpassDescription(const VULKAN_HPP_NAMESPACE::SubpassDescription& value) {
        object.subpassDescriptions.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder& RenderPass::Builder::setSubpassDependencies(const std::vector<VULKAN_HPP_NAMESPACE::SubpassDependency>& value) {
        object.subpassDependencies = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder& RenderPass::Builder::addSubpassDependency(const VULKAN_HPP_NAMESPACE::SubpassDependency& value) {
        object.subpassDependencies.emplace_back(value);
        return *this;
    }

    EXQUDENS_VULKAN_INLINE RenderPass::Builder& RenderPass::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::RenderPassCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE RenderPass& RenderPass::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            object.createInfo.attachmentCount = static_cast<uint32_t>(object.attachmentDescriptions.size());
            object.createInfo.pAttachments = object.attachmentDescriptions.empty() ? nullptr : object.attachmentDescriptions.data();
            if (!object.subpassDescriptions.empty()) {
                if (object.subpassDescriptions.size() != object.subpassDescriptionAttachmentReferences.size()) {
                    throw std::runtime_error(CALL_INFO + ": 'subpassDescriptions.size' not equal to 'subpassDescriptionAttachmentReferences.size'");
                }
                for (size_t i = 0; i < object.subpassDescriptions.size(); i++) {
                    if (!object.subpassDescriptionAttachmentReferences.at(i).empty()) {
                        object.subpassDescriptions.at(i).colorAttachmentCount = static_cast<uint32_t>(object.subpassDescriptionAttachmentReferences.at(i).size());
                        object.subpassDescriptions.at(i).pColorAttachments = object.subpassDescriptionAttachmentReferences.at(i).empty() ? nullptr : object.subpassDescriptionAttachmentReferences.at(i).data();
                    }
                }
            }
            object.createInfo.subpassCount = static_cast<uint32_t>(object.subpassDescriptions.size());
            object.createInfo.pSubpasses = object.subpassDescriptions.empty() ? nullptr :object.subpassDescriptions.data();
            object.createInfo.dependencyCount = static_cast<uint32_t>(object.subpassDependencies.size());
            object.createInfo.pDependencies = object.subpassDependencies.empty() ? nullptr : object.subpassDependencies.data();
            object.target = device.createRenderPass(object.createInfo);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
