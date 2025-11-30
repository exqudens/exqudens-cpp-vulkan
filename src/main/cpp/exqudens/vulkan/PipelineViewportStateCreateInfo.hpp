#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT PipelineViewportStateCreateInfo: vk::PipelineViewportStateCreateInfo {

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineViewportStateCreateInfo& setFlags(const vk::PipelineViewportStateCreateFlags& val);

    std::vector<vk::Viewport> viewports;
    std::vector<vk::Rect2D> scissors;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineViewportStateCreateInfo& addViewport(const vk::Viewport& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineViewportStateCreateInfo& setViewports(const std::vector<vk::Viewport>& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineViewportStateCreateInfo& addScissor(const vk::Rect2D& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES PipelineViewportStateCreateInfo& setScissors(const std::vector<vk::Rect2D>& val);

  };

}
