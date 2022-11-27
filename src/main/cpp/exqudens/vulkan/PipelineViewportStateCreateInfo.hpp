#pragma once

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT PipelineViewportStateCreateInfo: vk::PipelineViewportStateCreateInfo {

    PipelineViewportStateCreateInfo& setFlags(const vk::PipelineViewportStateCreateFlags& val);

    std::vector<vk::Viewport> viewports;
    std::vector<vk::Rect2D> scissors;

    PipelineViewportStateCreateInfo& addViewport(const vk::Viewport& val);

    PipelineViewportStateCreateInfo& setViewports(const std::vector<vk::Viewport>& val);

    PipelineViewportStateCreateInfo& addScissor(const vk::Rect2D& val);

    PipelineViewportStateCreateInfo& setScissors(const std::vector<vk::Rect2D>& val);

  };

}
