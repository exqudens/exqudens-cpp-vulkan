#pragma once

#include <vector>

#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Context.hpp"

namespace exqudens::vulkan {

  struct LightContext {

    std::vector<Image> shadowImages = {};
    std::vector<ImageView> shadowImageViews = {};
    Sampler sampler = {};
    DescriptorSetLayout descriptorSetLayout = {};
    DescriptorPool descriptorPool = {};
    std::vector<DescriptorSet> descriptorSets = {};
    RenderPass renderPass = {};
    Pipeline pipeline = {};
    std::vector<Framebuffer> framebuffers = {};

    void init(
        Context& root,
        std::vector<Buffer>& uniformBuffers,
        const std::vector<std::string>& paths
    );

  };

}
