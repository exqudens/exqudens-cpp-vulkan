#pragma once

#include <vector>

#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Context.hpp"
#include "exqudens/vulkan/LightContext.hpp"

namespace exqudens::vulkan {

  struct CameraContext {

    DescriptorSetLayout descriptorSetLayout = {};
    Sampler sampler = {};
    DescriptorPool descriptorPool = {};
    std::vector<DescriptorSet> descriptorSets = {};

    Image colorImage = {};
    ImageView colorImageView = {};
    Image depthImage = {};
    ImageView depthImageView = {};
    RenderPass renderPass = {};
    Pipeline pipeline = {};
    std::vector<Framebuffer> framebuffers = {};

    void init(
        Context& root,
        LightContext& light,
        uint32_t& mipLevels,
        std::vector<Buffer>& uniformBuffers,
        ImageView& textureImageView
    );

    void initSwapchain(
        Context& root,
        const std::vector<std::string>& paths
    );

  };

}
