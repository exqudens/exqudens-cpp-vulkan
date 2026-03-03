#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <optional>
#include <array>
#include <vector>
#include <set>
#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <iostream>

#include <gtest/gtest.h>
#include <exqudens/Log.hpp>
#include <exqudens/log/api/Logging.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan_raii.hpp>

#include "TestUtils.hpp"
#include "exqudens/vulkan.hpp"
#include "exqudens/vulkan/ShaderModule.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

class VulkanTutorialCom3GuiTests: public testing::Test {

    public:

        inline static const char* LOGGER_ID = "VulkanTutorialCom3GuiTests";

    private:

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type,
            const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        ) {
            if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
                if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
                    EXQUDENS_LOG_TRACE(LOGGER_ID) << pCallbackData->pMessage;
                } else if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) {
                    EXQUDENS_LOG_INFO(LOGGER_ID) << pCallbackData->pMessage;
                } else if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
                    EXQUDENS_LOG_WARNING(LOGGER_ID) << pCallbackData->pMessage;
                } else if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
                    EXQUDENS_LOG_ERROR(LOGGER_ID) << pCallbackData->pMessage;
                }
            }
            return vk::False;
        }

    protected:

        struct Vertex {
            glm::vec2 pos;
            glm::vec3 color;

            static vk::VertexInputBindingDescription getBindingDescription() {
                vk::VertexInputBindingDescription bindingDescription = {};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = vk::VertexInputRate::eVertex;
                return bindingDescription;
            }

            static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
                std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions = {};
                attributeDescriptions.at(0).binding = 0;
                attributeDescriptions.at(0).location = 0;
                attributeDescriptions.at(0).format = vk::Format::eR32G32Sfloat;
                attributeDescriptions.at(0).offset = offsetof(Vertex, pos);
                attributeDescriptions.at(1).binding = 0;
                attributeDescriptions.at(1).location = 1;
                attributeDescriptions.at(1).format = vk::Format::eR32G32B32Sfloat;
                attributeDescriptions.at(1).offset = offsetof(Vertex, color);
                return attributeDescriptions;
            }
        };

        struct UniformBufferObject {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 proj;
        };

        class Application {

            public:

                std::optional<std::string> testInputDir = {};
                std::optional<std::string> testOutputDir = {};

            private:

                const std::vector<Vertex> vertices = {
                    /*{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}*/

                    /*{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}*/

                    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
                };

                const std::vector<uint16_t> indices = {
                    0, 1, 2, 2, 3, 0
                };

                int width = 0;
                int height = 0;
                const char* title = nullptr;
#ifdef NDEBUG
                bool enableValidationLayers = false;
#else
                bool enableValidationLayers = true;
#endif
                GLFWwindow* window = nullptr;

                vk::PhysicalDeviceFeatures deviceFeatures;

                vk::raii::Context context;
                exqudens::vulkan::Instance instance = {};
                exqudens::vulkan::DebugUtilsMessenger debugUtilsMessenger = {};
                exqudens::vulkan::Surface surface = {};
                exqudens::vulkan::PhysicalDevice physicalDevice = {};
                exqudens::vulkan::Device device = {};
                exqudens::vulkan::Queue transferQueue = {};
                exqudens::vulkan::Queue graphicsQueue = {};
                exqudens::vulkan::Queue presentQueue = {};
                exqudens::vulkan::DescriptorSetLayout descriptorSetLayout = {};
                exqudens::vulkan::PipelineLayout pipelineLayout = {};
                exqudens::vulkan::PipelineCache pipelineCache = {};
                exqudens::vulkan::ShaderModule vertShaderModule = {};
                exqudens::vulkan::ShaderModule fragShaderModule = {};
                exqudens::vulkan::Buffer vertexStagingBuffer = {};
                exqudens::vulkan::DeviceMemory vertexStagingBufferMemory = {};
                exqudens::vulkan::Buffer vertexBuffer = {};
                exqudens::vulkan::DeviceMemory vertexBufferMemory = {};
                exqudens::vulkan::Buffer indexStagingBuffer = {};
                exqudens::vulkan::DeviceMemory indexStagingBufferMemory = {};
                exqudens::vulkan::Buffer indexBuffer = {};
                exqudens::vulkan::DeviceMemory indexBufferMemory = {};
                exqudens::vulkan::CommandPool transferCommandPool = {};
                exqudens::vulkan::CommandBuffers transferCommandBuffers = {};
                exqudens::vulkan::CommandPool graphicsCommandPool = {};

                exqudens::vulkan::Swapchain swapchain = {};
                exqudens::vulkan::DescriptorPool descriptorPool = {};
                exqudens::vulkan::DescriptorSets descriptorSets = {};
                std::vector<exqudens::vulkan::Buffer> uniformBuffers = {};
                std::vector<exqudens::vulkan::DeviceMemory> uniformBufferMemories = {};
                std::vector<void*> uniformBufferMappedMemories = {};
                std::vector<exqudens::vulkan::ImageView> imageViews = {};
                exqudens::vulkan::RenderPass renderPass = {};
                exqudens::vulkan::Pipeline pipeline = {};
                std::vector<exqudens::vulkan::Framebuffer> framebuffers = {};
                exqudens::vulkan::CommandBuffers graphicsCommandBuffers = {};
                std::vector<exqudens::vulkan::Semaphore> imageAvailableSemaphores = {};
                std::vector<exqudens::vulkan::Semaphore> renderFinishedSemaphores = {};
                std::vector<exqudens::vulkan::Fence> inFlightFences = {};

                bool framebufferResized = false;
                uint32_t currentFrame = 0;

            public:

                void run() {
                    try {
                        initWindow();
                        initVulkan();
                        loop();
                        cleanup();
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

            private:

                void initWindow() {
                    try {
                        glfwInit();

                        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

                        width = 800;
                        height = 600;
                        title = "Vulkan";
                        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
                        glfwSetWindowUserPointer(window, this);
                        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

                static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
                    try {
                        auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
                        app->framebufferResized = true;
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

                void loop() {
                    try {
                        while (!glfwWindowShouldClose(window)) {
                            glfwPollEvents();
                            drawFrame();
                        }

                        device.target.waitIdle();
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

                void cleanup() {
                    try {
                        glfwDestroyWindow(window);
                        glfwTerminate();
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

                void initVulkan() {
                    try {
                        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

                        uint32_t glfwExtensionCount = 0;
                        const char** glfwExtensions = nullptr;
                        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
                        std::vector<const char*> instanceRequiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

                        std::vector<const char*> deviceRequiredExtensions = {
                            vk::KHRSwapchainExtensionName
                        };
                        std::vector<const char*> requiredLayers = {};

                        if (enableValidationLayers) {
                            instanceRequiredExtensions.emplace_back(vk::EXTDebugUtilsExtensionName);
                            requiredLayers.emplace_back("VK_LAYER_KHRONOS_validation");
                        }

                        //deviceFeatures = vk::PhysicalDeviceFeatures().setSamplerAnisotropy(true).setSampleRateShading(true);

                        int framebufferWidth = 0;
                        int framebufferHeight = 0;
                        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

                        exqudens::vulkan::Instance::builder(instance)
                        .setApplicationInfo(
                            vk::ApplicationInfo()
                            .setApiVersion(vk::ApiVersion14)
                            .setPApplicationName(LOGGER_ID)
                            .setApplicationVersion(VK_MAKE_VERSION(0, 0, 1))
                            .setPEngineName("No Engine")
                            .setEngineVersion(VK_MAKE_VERSION(0, 0, 1))
                        )
                        .setEnabledExtensionNames(instanceRequiredExtensions)
                        .setEnabledLayerNames(requiredLayers)
                        .build(context);

                        exqudens::vulkan::DebugUtilsMessenger::builder(debugUtilsMessenger)
                        .setCreateInfo(
                            vk::DebugUtilsMessengerCreateInfoEXT()
                            .setMessageSeverity(
                                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
                                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                            )
                            .setMessageType(
                                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                                | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                                | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                                //| vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
                            )
                            .setPfnUserCallback(&VulkanTutorialCom3GuiTests::debugCallback)
                        )
                        .build(instance.target);

                        VkSurfaceKHR vkSurface = nullptr;
                        if (glfwCreateWindowSurface(*instance.target, window, nullptr, &vkSurface) != VK_SUCCESS || vkSurface == nullptr) {
                            throw std::runtime_error(CALL_INFO + ": failed to create surface!");
                        }

                        exqudens::vulkan::Surface::builder(surface)
                        .setSurface(vkSurface)
                        .build(instance.target);

                        std::optional<uint32_t> transferIndex = {};
                        std::optional<uint32_t> graphicsIndex = {};
                        std::optional<uint32_t> presentIndex = {};

                        std::vector<vk::raii::PhysicalDevice> devices = instance.target.enumeratePhysicalDevices();
                        for (auto& d : devices) {
                            std::vector<vk::QueueFamilyProperties> queueFamilyProperties = d.getQueueFamilyProperties();
                            for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
                                if (
                                    (queueFamilyProperties.at(i).queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer
                                    && (queueFamilyProperties.at(i).queueFlags & vk::QueueFlagBits::eGraphics) == static_cast<vk::QueueFlags>(0)
                                ) {
                                    transferIndex = static_cast<uint32_t>(i);
                                }
                                if (
                                    (queueFamilyProperties.at(i).queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0)
                                    && d.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface.surface)
                                ) {
                                    graphicsIndex = static_cast<uint32_t>(i);
                                    presentIndex = graphicsIndex.value();
                                }
                                if (transferIndex && graphicsIndex && presentIndex) {
                                    break;
                                }
                            }
                            std::set<std::string> notFoundExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());
                            std::vector<vk::ExtensionProperties> extensionProperties = d.enumerateDeviceExtensionProperties();
                            for (const auto& ep : extensionProperties) {
                                notFoundExtensions.erase(ep.extensionName);
                            }
                            if (transferIndex && graphicsIndex && presentIndex && notFoundExtensions.empty()) {
                                exqudens::vulkan::PhysicalDevice::builder(physicalDevice)
                                .setRequiredExtensions(deviceRequiredExtensions)
                                .build(d);
                                break;
                            }
                        }
                        if (physicalDevice.target == nullptr) {
                            throw std::runtime_error(CALL_INFO + ": failed to create physical device!");
                        }

                        float queuePriority = 1.0f;

                        exqudens::vulkan::Device::builder(device)
                        .addUniqQueueCreateInfos(
                            vk::DeviceQueueCreateInfo()
                            .setQueueFamilyIndex(transferIndex.value())
                            .setQueueCount(1)
                            .setPQueuePriorities(&queuePriority)
                        )
                        .addUniqQueueCreateInfos(
                            vk::DeviceQueueCreateInfo()
                            .setQueueFamilyIndex(graphicsIndex.value())
                            .setQueueCount(1)
                            .setPQueuePriorities(&queuePriority)
                        )
                        .addUniqQueueCreateInfos(
                            vk::DeviceQueueCreateInfo()
                            .setQueueFamilyIndex(presentIndex.value())
                            .setQueueCount(1)
                            .setPQueuePriorities(&queuePriority)
                        )
                        .setCreateInfo(
                            vk::DeviceCreateInfo()
                            .setEnabledExtensionCount(static_cast<uint32_t>(physicalDevice.requiredExtensions.size()))
                            .setPpEnabledExtensionNames(physicalDevice.requiredExtensions.data())
                            .setPEnabledFeatures(&deviceFeatures)
                        )
                        .build(physicalDevice.target);

                        exqudens::vulkan::Queue::builder(transferQueue)
                        .setFamilyIndex(transferIndex.value())
                        .build(device.target);

                        exqudens::vulkan::Queue::builder(graphicsQueue)
                        .setFamilyIndex(graphicsIndex.value())
                        .build(device.target);

                        exqudens::vulkan::Queue::builder(presentQueue)
                        .setFamilyIndex(presentIndex.value())
                        .build(device.target);

                        exqudens::vulkan::DescriptorSetLayout::builder(descriptorSetLayout)
                        .addBinding(
                            vk::DescriptorSetLayoutBinding()
                            .setBinding(0)
                            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                            .setDescriptorCount(1)
                            .setStageFlags(vk::ShaderStageFlagBits::eVertex)
                            .setPImmutableSamplers(nullptr)
                        )
                        .build(device.target);

                        exqudens::vulkan::PipelineLayout::builder(pipelineLayout)
                        .setCreateInfo(
                            vk::PipelineLayoutCreateInfo()
                            .setSetLayoutCount(1)
                            .setPSetLayouts(&*descriptorSetLayout.target)
                            .setPushConstantRangeCount(0)
                        )
                        .build(device.target);

                        exqudens::vulkan::PipelineCache::builder(pipelineCache)
                        .build(device.target);

                        exqudens::vulkan::ShaderModule::builder(vertShaderModule)
                        .setFile(
                            std::filesystem::path(testOutputDir.value())
                            .append("vert.spv")
                            .generic_string()
                            .c_str()
                        )
                        .setReadFile(true)
                        .build(device.target);

                        exqudens::vulkan::ShaderModule::builder(fragShaderModule)
                        .setFile(
                            std::filesystem::path(testOutputDir.value())
                            .append("frag.spv")
                            .generic_string()
                            .c_str()
                        )
                        .setReadFile(true)
                        .build(device.target);

                        exqudens::vulkan::Buffer::builder(vertexStagingBuffer)
                        .setCreateInfo(
                            vk::BufferCreateInfo()
                            .setSize(sizeof(vertices.at(0)) * vertices.size())
                            .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                            .setSharingMode(vk::SharingMode::eExclusive)
                        )
                        .build(device.target);

                        exqudens::vulkan::DeviceMemory::builder(vertexStagingBufferMemory)
                        .setAllocateInfo(
                            exqudens::vulkan::DeviceMemory::allocateInfoFrom(
                                physicalDevice.target,
                                vertexStagingBuffer.target,
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                            )
                        )
                        .build(device.target);

                        vertexStagingBuffer.target.bindMemory(*vertexStagingBufferMemory.target, 0);

                        exqudens::vulkan::Buffer::builder(vertexBuffer)
                        .setCreateInfo(
                            vk::BufferCreateInfo()
                            .setSize(sizeof(vertices.at(0)) * vertices.size())
                            .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer)
                            .setSharingMode(vk::SharingMode::eExclusive)
                        )
                        .build(device.target);

                        exqudens::vulkan::DeviceMemory::builder(vertexBufferMemory)
                        .setAllocateInfo(
                            exqudens::vulkan::DeviceMemory::allocateInfoFrom(
                                physicalDevice.target,
                                vertexBuffer.target,
                                vk::MemoryPropertyFlagBits::eDeviceLocal
                            )
                        )
                        .build(device.target);

                        vertexBuffer.target.bindMemory(*vertexBufferMemory.target, 0);

                        exqudens::vulkan::Buffer::builder(indexStagingBuffer)
                        .setCreateInfo(
                            vk::BufferCreateInfo()
                            .setSize(sizeof(indices.at(0)) * indices.size())
                            .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                            .setSharingMode(vk::SharingMode::eExclusive)
                        )
                        .build(device.target);

                        exqudens::vulkan::DeviceMemory::builder(indexStagingBufferMemory)
                        .setAllocateInfo(
                            exqudens::vulkan::DeviceMemory::allocateInfoFrom(
                                physicalDevice.target,
                                indexStagingBuffer.target,
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                            )
                        )
                        .build(device.target);

                        indexStagingBuffer.target.bindMemory(*indexStagingBufferMemory.target, 0);

                        exqudens::vulkan::Buffer::builder(indexBuffer)
                        .setCreateInfo(
                            vk::BufferCreateInfo()
                            .setSize(sizeof(indices.at(0)) * indices.size())
                            .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer)
                            .setSharingMode(vk::SharingMode::eExclusive)
                        )
                        .build(device.target);

                        exqudens::vulkan::DeviceMemory::builder(indexBufferMemory)
                        .setAllocateInfo(
                            exqudens::vulkan::DeviceMemory::allocateInfoFrom(
                                physicalDevice.target,
                                indexBuffer.target,
                                vk::MemoryPropertyFlagBits::eDeviceLocal
                            )
                        )
                        .build(device.target);

                        indexBuffer.target.bindMemory(*indexBufferMemory.target, 0);

                        exqudens::vulkan::CommandPool::builder(transferCommandPool)
                        .setCreateInfo(
                            vk::CommandPoolCreateInfo()
                            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                            .setQueueFamilyIndex(transferQueue.familyIndex.value())
                        )
                        .build(device.target);

                        exqudens::vulkan::CommandBuffers::builder(transferCommandBuffers)
                        .setAllocateInfo(
                            vk::CommandBufferAllocateInfo()
                            .setCommandPool(*transferCommandPool.target)
                            .setLevel(vk::CommandBufferLevel::ePrimary)
                            .setCommandBufferCount(1)
                        )
                        .build(device.target);

                        exqudens::vulkan::CommandPool::builder(graphicsCommandPool)
                        .setCreateInfo(
                            vk::CommandPoolCreateInfo()
                            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                            .setQueueFamilyIndex(graphicsQueue.familyIndex.value())
                        )
                        .build(device.target);

                        vertexStagingBufferMemory.fill(vertices.data());
                        indexStagingBufferMemory.fill(indices.data());

                        transferCommandBuffers.targets.at(0).begin({});
                        transferCommandBuffers.targets.at(0).copyBuffer(
                            *vertexStagingBuffer.target,
                            *vertexBuffer.target,
                            {vk::BufferCopy().setSize(vertexBuffer.createInfo.value().size)}
                        );
                        transferCommandBuffers.targets.at(0).copyBuffer(
                            *indexStagingBuffer.target,
                            *indexBuffer.target,
                            {vk::BufferCopy().setSize(indexBuffer.createInfo.value().size)}
                        );
                        transferCommandBuffers.targets.at(0).end();
                        transferQueue.target.submit(
                            vk::SubmitInfo()
                            .setCommandBufferCount(1)
                            .setPCommandBuffers(&(*transferCommandBuffers.targets.at(0)))
                        );
                        transferQueue.target.waitIdle();

                        vertexStagingBuffer.clearAndRelease();
                        vertexStagingBufferMemory.clearAndRelease();

                        indexStagingBuffer.clearAndRelease();
                        indexStagingBufferMemory.clearAndRelease();

                        initVulkanSwapchain(framebufferWidth, framebufferHeight);

                        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

                void initVulkanSwapchain(int swapchainWidth, int swapchainHeight) {
                    try {
                        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

                        exqudens::vulkan::Swapchain::builder(swapchain)
                        .setCreateInfo(
                            exqudens::vulkan::Swapchain::createInfoFrom(
                                physicalDevice.target,
                                surface.target,
                                swapchainWidth,
                                swapchainHeight
                            )
                            .setImageArrayLayers(1)
                            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                            .setClipped(true)
                            .setSurface(*surface.target)
                        )
                        .setQueueFamilyIndices({
                            graphicsQueue.familyIndex.value(),
                            presentQueue.familyIndex.value()
                        })
                        .build(device.target);

                        exqudens::vulkan::DescriptorPool::builder(descriptorPool)
                        .addSize(
                            vk::DescriptorPoolSize()
                            .setType(vk::DescriptorType::eUniformBuffer)
                            .setDescriptorCount(static_cast<uint32_t>(swapchain.target.getImages().size()))
                        )
                        .setCreateInfo(
                            vk::DescriptorPoolCreateInfo()
                            .setMaxSets(static_cast<uint32_t>(swapchain.target.getImages().size()))
                            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                        )
                        .build(device.target);

                        exqudens::vulkan::DescriptorSets::builder(descriptorSets)
                        .setLayouts(std::vector<vk::DescriptorSetLayout>(swapchain.target.getImages().size(), *descriptorSetLayout.target))
                        .setAllocateInfo(
                            vk::DescriptorSetAllocateInfo()
                            .setDescriptorPool(*descriptorPool.target)
                        )
                        .build(device.target);

                        uniformBuffers.resize(swapchain.target.getImages().size());
                        uniformBufferMemories.resize(swapchain.target.getImages().size());
                        uniformBufferMappedMemories.resize(swapchain.target.getImages().size());
                        imageViews.resize(swapchain.target.getImages().size());
                        for (size_t i = 0; i < swapchain.target.getImages().size(); i++) {
                            exqudens::vulkan::Buffer::builder(uniformBuffers.at(i))
                            .setCreateInfo(
                                vk::BufferCreateInfo()
                                .setSize(sizeof(UniformBufferObject))
                                .setUsage(vk::BufferUsageFlagBits::eUniformBuffer)
                                .setSharingMode(vk::SharingMode::eExclusive)
                            )
                            .build(device.target);

                            exqudens::vulkan::DeviceMemory::builder(uniformBufferMemories.at(i))
                            .setAllocateInfo(
                                exqudens::vulkan::DeviceMemory::allocateInfoFrom(
                                    physicalDevice.target,
                                    uniformBuffers.at(i).target,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                                )
                            )
                            .build(device.target);

                            uniformBuffers.at(i).target.bindMemory(*uniformBufferMemories.at(i).target, 0);

                            uniformBufferMappedMemories.at(i) = uniformBufferMemories.at(i).target.mapMemory(0, uniformBufferMemories.at(i).allocateInfo->allocationSize);

                            exqudens::vulkan::ImageView::builder(imageViews.at(i))
                            .setCreateInfo(
                                vk::ImageViewCreateInfo()
                                .setImage(swapchain.target.getImages().at(i))
                                .setViewType(vk::ImageViewType::e2D)
                                .setFormat(swapchain.createInfo.value().imageFormat)
                                .setComponents(
                                    vk::ComponentMapping()
                                    .setR(vk::ComponentSwizzle::eIdentity)
                                    .setG(vk::ComponentSwizzle::eIdentity)
                                    .setB(vk::ComponentSwizzle::eIdentity)
                                    .setA(vk::ComponentSwizzle::eIdentity)
                                )
                                .setSubresourceRange(
                                    vk::ImageSubresourceRange()
                                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                    .setBaseMipLevel(0)
                                    .setLevelCount(1)
                                    .setBaseArrayLayer(0)
                                    .setLayerCount(1)
                                )
                            )
                            .build(device.target);

                            vk::DescriptorBufferInfo descriptorBufferInfo = vk::DescriptorBufferInfo()
                            .setBuffer(*uniformBuffers.at(i).target)
                            .setOffset(0)
                            .setRange(sizeof(UniformBufferObject));

                            vk::WriteDescriptorSet writeDescriptorSet = vk::WriteDescriptorSet()
                            .setDstSet(*descriptorSets.targets.at(i))
                            .setDstBinding(0)
                            .setDstArrayElement(0)
                            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                            .setDescriptorCount(1)
                            .setPBufferInfo(&descriptorBufferInfo);

                            device.target.updateDescriptorSets({writeDescriptorSet}, {});
                        }

                        exqudens::vulkan::RenderPass::builder(renderPass)
                        .addAttachmentDescription(
                            vk::AttachmentDescription()
                            .setFormat(swapchain.createInfo.value().imageFormat)
                            .setSamples(vk::SampleCountFlagBits::e1)
                            .setLoadOp(vk::AttachmentLoadOp::eClear)
                            .setStoreOp(vk::AttachmentStoreOp::eStore)
                            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                            .setInitialLayout(vk::ImageLayout::eUndefined)
                            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
                        )
                        .addSubpassDescriptionAttachmentReferences({
                            vk::AttachmentReference()
                            .setAttachment(0)
                            .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
                        })
                        .addSubpassDependency(
                            vk::SubpassDependency()
                            .setSrcSubpass(vk::SubpassExternal)
                            .setDstSubpass(0)
                            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                            .setSrcAccessMask(vk::AccessFlagBits::eNone)
                            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                        )
                        .addSubpassDescription(
                            vk::SubpassDescription()
                            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                        )
                        .setCreateInfo(
                            vk::RenderPassCreateInfo()
                        )
                        .build(device.target);

                        exqudens::vulkan::Pipeline::builder(pipeline)
                        .addShaderStageCreateInfo(
                            vk::PipelineShaderStageCreateInfo()
                            .setModule(vertShaderModule.target)
                            .setStage(vk::ShaderStageFlagBits::eVertex)
                            .setPName("main")
                        )
                        .addShaderStageCreateInfo(
                            vk::PipelineShaderStageCreateInfo()
                            .setModule(fragShaderModule.target)
                            .setStage(vk::ShaderStageFlagBits::eFragment)
                            .setPName("main")
                        )
                        .setVertexInputStateCreateInfoBindings({
                            Vertex::getBindingDescription()
                        })
                        .setVertexInputStateCreateInfoAttributes({
                            Vertex::getAttributeDescriptions().at(0),
                            Vertex::getAttributeDescriptions().at(1)
                        })
                        .setVertexInputStateCreateInfo(
                            vk::PipelineVertexInputStateCreateInfo()
                        )
                        .setInputAssemblyStateCreateInfo(
                            vk::PipelineInputAssemblyStateCreateInfo()
                            .setTopology(vk::PrimitiveTopology::eTriangleList)
                            .setPrimitiveRestartEnable(false)
                        )
                        .setViewportStateCreateInfo(
                            vk::PipelineViewportStateCreateInfo()
                            .setViewportCount(1)
                            .setScissorCount(1)
                        )
                        .setRasterizationStateCreateInfo(
                            vk::PipelineRasterizationStateCreateInfo()
                            .setFlags({})
                            .setDepthClampEnable(false)
                            .setRasterizerDiscardEnable(false)
                            .setPolygonMode(vk::PolygonMode::eFill)
                            .setCullMode(vk::CullModeFlagBits::eBack)
                            .setFrontFace(vk::FrontFace::eCounterClockwise)
                            .setDepthBiasEnable(false)
                            .setDepthBiasConstantFactor(0.0f)
                            .setDepthBiasClamp(0.0f)
                            .setDepthBiasSlopeFactor(1.0f)
                            .setLineWidth(1.0f)
                        )
                        .setMultisampleStateCreateInfo(
                            vk::PipelineMultisampleStateCreateInfo()
                            .setSampleShadingEnable(false)
                            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
                        )
                        .addColorBlendAttachmentState(
                            vk::PipelineColorBlendAttachmentState()
                            .setColorWriteMask(
                                vk::ColorComponentFlagBits::eR
                                | vk::ColorComponentFlagBits::eG
                                | vk::ColorComponentFlagBits::eB
                                | vk::ColorComponentFlagBits::eA
                            )
                            .setBlendEnable(false)
                        )
                        .setColorBlendStateCreateInfo(
                            vk::PipelineColorBlendStateCreateInfo()
                            .setLogicOpEnable(false)
                            .setLogicOp(VULKAN_HPP_NAMESPACE::LogicOp::eCopy)
                            .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f})
                        )
                        .addDynamicState(vk::DynamicState::eViewport)
                        .addDynamicState(vk::DynamicState::eScissor)
                        .setRenderingCreateInfo(
                            vk::PipelineRenderingCreateInfo()
                            .setColorAttachmentCount(1)
                            .setPColorAttachmentFormats(&swapchain.createInfo.value().imageFormat)
                        )
                        .setGraphicsCreateInfo(
                            vk::GraphicsPipelineCreateInfo()
                            .setLayout(*pipelineLayout.target)
                            .setRenderPass(*renderPass.target)
                            .setSubpass(0)
                        )
                        .build(device.target, pipelineCache.target);

                        framebuffers.resize(swapchain.target.getImages().size());
                        for (size_t i = 0; i < swapchain.target.getImages().size(); i++) {
                            exqudens::vulkan::Framebuffer::builder(framebuffers.at(i))
                            .addAttachment(*imageViews.at(i).target)
                            .setCreateInfo(
                                vk::FramebufferCreateInfo()
                                .setRenderPass(*renderPass.target)
                                .setWidth(swapchain.createInfo.value().imageExtent.width)
                                .setHeight(swapchain.createInfo.value().imageExtent.height)
                                .setLayers(1)
                            )
                            .build(device.target);
                        }

                        exqudens::vulkan::CommandBuffers::builder(graphicsCommandBuffers)
                        .setAllocateInfo(
                            vk::CommandBufferAllocateInfo()
                            .setCommandPool(*graphicsCommandPool.target)
                            .setLevel(vk::CommandBufferLevel::ePrimary)
                            .setCommandBufferCount(static_cast<uint32_t>(swapchain.target.getImages().size()))
                        )
                        .build(device.target);

                        imageAvailableSemaphores.resize(swapchain.target.getImages().size());
                        renderFinishedSemaphores.resize(swapchain.target.getImages().size());
                        inFlightFences.resize(swapchain.target.getImages().size());
                        for (size_t i = 0; i < swapchain.target.getImages().size(); i++) {
                            exqudens::vulkan::Semaphore::builder(imageAvailableSemaphores.at(i)).build(device.target);
                            exqudens::vulkan::Semaphore::builder(renderFinishedSemaphores.at(i)).build(device.target);
                            exqudens::vulkan::Fence::builder(inFlightFences.at(i)).setCreateInfo(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled)).build(device.target);
                        }

                        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

                void recreateSwapChain() {
                    try {
                        int framebufferWidth = 0, framebufferHeight = 0;
                        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
                        while (framebufferWidth == 0 || height == 0) {
                            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
                            glfwWaitEvents();
                        }

                        device.target.waitIdle();

                        for (size_t i = 0; i < inFlightFences.size(); i++) {
                            inFlightFences.at(i).clearAndRelease();
                        }

                        for (size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
                            renderFinishedSemaphores.at(i).clearAndRelease();
                        }

                        for (size_t i = 0; i < imageAvailableSemaphores.size(); i++) {
                            imageAvailableSemaphores.at(i).clearAndRelease();
                        }

                        graphicsCommandBuffers.clearAndRelease();

                        for (size_t i = 0; i < framebuffers.size(); i++) {
                            framebuffers.at(i).clearAndRelease();
                        }

                        pipeline.clearAndRelease();
                        renderPass.clearAndRelease();

                        for (size_t i = 0; i < imageViews.size(); i++) {
                            imageViews.at(i).clearAndRelease();
                        }

                        for (size_t i = 0; i < uniformBuffers.size(); i++) {
                            uniformBuffers.at(i).clearAndRelease();
                        }

                        for (size_t i = 0; i < uniformBufferMemories.size(); i++) {
                            uniformBufferMemories.at(i).clearAndRelease();
                        }

                        descriptorSets.clearAndRelease();
                        descriptorPool.clearAndRelease();

                        swapchain.clearAndRelease();

                        initVulkanSwapchain(framebufferWidth, framebufferHeight);
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

                void recordCommandBuffer(vk::raii::CommandBuffer& commandBuffer, size_t imageIndex) {
                    try {
                        commandBuffer.begin(vk::CommandBufferBeginInfo());

                        vk::ClearValue clearValue = vk::ClearValue().setColor({0.0f, 0.0f, 0.0f, 1.0f});
                        commandBuffer.beginRenderPass(
                            vk::RenderPassBeginInfo()
                            .setRenderPass(*renderPass.target)
                            .setFramebuffer(*framebuffers.at(imageIndex).target)
                            .setRenderArea(
                                vk::Rect2D()
                                .setOffset(vk::Offset2D()
                                    .setX(0)
                                    .setY(0)
                                )
                                .setExtent(swapchain.createInfo.value().imageExtent)
                            )
                            .setClearValueCount(1)
                            .setPClearValues(&clearValue),

                            vk::SubpassContents::eInline
                        );

                        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.target);

                        commandBuffer.setViewport(0, {
                            vk::Viewport()
                            .setX(0.0f)
                            .setY(0.0f)
                            .setWidth(static_cast<float>(swapchain.createInfo.value().imageExtent.width))
                            .setHeight(static_cast<float>(swapchain.createInfo.value().imageExtent.height))
                            .setMinDepth(0.0f)
                            .setMaxDepth(1.0f)
                        });

                        commandBuffer.setScissor(0, {
                            vk::Rect2D()
                            .setOffset(vk::Offset2D().setY(0).setY(0))
                            .setExtent(swapchain.createInfo.value().imageExtent)
                        });

                        commandBuffer.bindVertexBuffers(0, {*vertexBuffer.target}, {0});
                        commandBuffer.bindIndexBuffer(*indexBuffer.target, 0, vk::IndexType::eUint16);

                        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipelineLayout.target, 0, {*descriptorSets.targets.at(currentFrame)}, nullptr);
                        commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

                        commandBuffer.endRenderPass();
                        commandBuffer.end();
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

                void updateUniformBuffer(uint32_t currentFrame, uint32_t width, uint32_t height) {
                    static auto startTime = std::chrono::high_resolution_clock::now();
                    auto currentTime = std::chrono::high_resolution_clock::now();
                    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

                    UniformBufferObject ubo = {};
                    //ubo.model = {1.0f};
                    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                    ubo.view = {1.0f};
                    //ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                    ubo.proj = {1.0f};
                    //ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 10.0f);
                    ubo.proj[1][1] *= -1;

                    std::memcpy(uniformBufferMappedMemories.at(currentFrame), &ubo, sizeof(ubo));
                }

                void drawFrame() {
                    try {
                        (void) device.target.waitForFences({*inFlightFences.at(currentFrame).target}, true, UINT64_MAX);

                        vk::ResultValue<uint32_t> acquireNextImageValue = swapchain.target.acquireNextImage(UINT64_MAX, *imageAvailableSemaphores.at(currentFrame).target, nullptr);
                        uint32_t imageIndex = acquireNextImageValue.value;

                        if (acquireNextImageValue.result == vk::Result::eErrorOutOfDateKHR) {
                            recreateSwapChain();
                            return;
                        } else if (acquireNextImageValue.result != vk::Result::eSuccess && acquireNextImageValue.result != vk::Result::eSuboptimalKHR) {
                            throw std::runtime_error(CALL_INFO + ": failed to acquire swap chain image!");
                        }

                        updateUniformBuffer(currentFrame, swapchain.createInfo.value().imageExtent.width, swapchain.createInfo.value().imageExtent.height);

                        // Only reset the fence if we are submitting work
                        device.target.resetFences({*inFlightFences.at(currentFrame).target});

                        graphicsCommandBuffers.targets.at(currentFrame).reset();
                        recordCommandBuffer(graphicsCommandBuffers.targets.at(currentFrame), imageIndex);

                        std::vector<vk::Semaphore> waitSemaphores = {
                            *imageAvailableSemaphores.at(currentFrame).target
                        };
                        std::vector<vk::PipelineStageFlags> waitStages = {
                            vk::PipelineStageFlagBits::eColorAttachmentOutput
                        };
                        std::vector<vk::CommandBuffer> waitCommandBuffers = {
                            *graphicsCommandBuffers.targets.at(currentFrame)
                        };
                        std::vector<vk::Semaphore> signalSemaphores = {
                            *renderFinishedSemaphores.at(currentFrame).target
                        };

                        graphicsQueue.target.submit(
                            {
                                vk::SubmitInfo()
                                .setWaitSemaphores(waitSemaphores)
                                .setWaitDstStageMask(waitStages)
                                .setCommandBuffers(waitCommandBuffers)
                                .setSignalSemaphores(signalSemaphores)
                            },
                            *inFlightFences.at(currentFrame).target
                        );

                        std::vector<vk::SwapchainKHR> swapchains = {
                            *swapchain.target
                        };

                        vk::Result presentResult = presentQueue.target.presentKHR(
                            vk::PresentInfoKHR()
                            .setWaitSemaphores(signalSemaphores)
                            .setSwapchains(swapchains)
                            .setPImageIndices(&imageIndex)
                        );

                        if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR || framebufferResized) {
                            framebufferResized = false;
                            recreateSwapChain();
                        } else if (presentResult != vk::Result::eSuccess) {
                            throw std::runtime_error(CALL_INFO + ": failed to present swap chain image!");
                        }

                        currentFrame = (currentFrame + 1) % static_cast<uint32_t>(swapchain.target.getImages().size());
                    } catch (...) {
                        std::throw_with_nested(std::runtime_error(CALL_INFO));
                    }
                }

        };

};

/*
    @brief vulkan-tutorial.com/Vertex_buffers/Index_buffer

    @details pay-attention-to: vulkan-tutorial.com/Vertex_buffers/Staging_buffer section: Conclusion

    It should be noted that in a real world application, you're not supposed to actually call "vkAllocateMemory" for every
    individual buffer. The maximum number of simultaneous memory allocations is limited by the
    "maxMemoryAllocationCount" physical device limit, which may be as low as "4096" even on high-end hardware like an
    NVIDIA GTX 1080. The right way to allocate memory for a large number of objects at the same time is to create a
    custom allocator that splits up a single allocation among many different objects by using the "offset" parameters
    that we've seen in many functions.
*/
TEST_F(VulkanTutorialCom3GuiTests, test1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        Application app;

        app.testInputDir = TestUtils::getTestInputDir(testGroup, testCase);
        app.testOutputDir = TestUtils::getTestOutputDir(testGroup, testCase);

        app.run();

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

#undef CALL_INFO
