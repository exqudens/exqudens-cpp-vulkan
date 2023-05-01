#pragma once

#include <vector>

#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Context.hpp"

namespace exqudens::vulkan {

  struct CameraContext {

    DescriptorSetLayout descriptorSetLayout = {};
    Sampler sampler = {};
    Image depthImage = {};
    ImageView depthImageView = {};
    Image colorImage = {};
    ImageView colorImageView = {};
    DescriptorPool descriptorPool = {};
    std::vector<DescriptorSet> descriptorSets = {};

    RenderPass renderPass = {};
    Pipeline pipeline = {};
    std::vector<Framebuffer> swapchainFramebuffers = {};

    void init(
        Context& root,
        uint32_t& mipLevels,
        std::vector<Buffer>& uniformBuffers,
        ImageView& textureImageView
    );

    void initSwapchain(
        Context& root,
        const std::vector<std::string> paths
    );

  };

}
