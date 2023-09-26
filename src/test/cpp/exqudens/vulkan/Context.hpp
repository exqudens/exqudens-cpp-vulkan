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
    std::vector<Image> swapchainImages = {};
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

    void init(
        const std::vector<const char*>& enabledExtensionNames,
        const std::vector<const char*>& enabledLayerNames,
        const std::function<VkSurfaceKHR(VkInstance)>& createSurfaceFunction,
        const uint32_t& queryPoolSize,
        const uint32_t& width,
        const uint32_t& height,
        const std::vector<std::string>& paths
    );

    void initSwapchain(
        const uint32_t& width,
        const uint32_t& height,
        const std::vector<std::string>& paths
    );

  };

}
