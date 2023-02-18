#pragma once

#include <vector>

#include "exqudens/vulkan/all.hpp"

namespace exqudens::vulkan {

  struct Context {

    Instance instance = {};
    Surface surface = {};
    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceHostQueryResetFeatures> physicalDeviceFeatures = {};
    PhysicalDevice physicalDevice = {};
    float physicalDeviceTimestampPeriod = 0.0f;
    vk::SampleCountFlagBits physicalDeviceMsaaSamples = {};
    Device device = {};
    Queue transferQueue = {};
    Queue graphicsQueue = {};
    Queue presentQueue = {};
    CommandPool transferCommandPool = {};
    CommandPool graphicsCommandPool = {};
    CommandBuffer transferCommandBuffer = {};
    DescriptorSetLayout descriptorSetLayout = {};
    QueryPool queryPool = {};
    Swapchain swapchain = {};
    std::vector<ImageView> swapchainImageViews = {};
    RenderPass renderPass = {};
    Pipeline pipeline = {};
    std::vector<Framebuffer> swapchainFramebuffers = {};
    std::vector<Image> samplerImages = {};
    std::vector<ImageView> samplerImageViews = {};
    DescriptorPool descriptorPool = {};
    Sampler sampler = {};
    std::vector<DescriptorSet> descriptorSets = {};
    std::vector<CommandBuffer> graphicsCommandBuffers = {};
    std::vector<Semaphore> imageAvailableSemaphores = {};
    std::vector<Semaphore> renderFinishedSemaphores = {};
    std::vector<Fence> inFlightFences = {};

  };

}
