#include "exqudens/vulkan/PipelineViewportStateCreateInfo.hpp"

namespace exqudens::vulkan {

  PipelineViewportStateCreateInfo& PipelineViewportStateCreateInfo::setFlags(const vk::PipelineViewportStateCreateFlags& val) {
    vk::PipelineViewportStateCreateInfo::setFlags(val);
    return *this;
  }

  PipelineViewportStateCreateInfo& PipelineViewportStateCreateInfo::addViewport(const vk::Viewport& val) {
    viewports.emplace_back(val);
    setViewports(viewports);
    return *this;
  }

  PipelineViewportStateCreateInfo& PipelineViewportStateCreateInfo::setViewports(const std::vector<vk::Viewport>& val) {
    viewports = val;
    vk::PipelineViewportStateCreateInfo::setViewports(viewports);
    return *this;
  }

  PipelineViewportStateCreateInfo& PipelineViewportStateCreateInfo::addScissor(const vk::Rect2D& val) {
    scissors.emplace_back(val);
    setScissors(scissors);
    return *this;
  }

  PipelineViewportStateCreateInfo& PipelineViewportStateCreateInfo::setScissors(const std::vector<vk::Rect2D>& val) {
    scissors = val;
    vk::PipelineViewportStateCreateInfo::setScissors(scissors);
    return *this;
  }

}
