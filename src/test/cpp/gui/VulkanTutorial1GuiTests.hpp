#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <set>
#include <filesystem>
#include <stdexcept>
#include <iostream>

#include <gtest/gtest.h>
#include <exqudens/Log.hpp>
#include <exqudens/log/api/Logging.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "TestUtils.hpp"
#include "TestGlfwUtils.hpp"
#include "exqudens/vulkan.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

class VulkanTutorial1GuiTests: public testing::Test {

    public:

        inline static const char* LOGGER_ID = "VulkanTutorial1GuiTests";

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

        class Application {

            public:

                std::optional<std::string> testInputDir = {};
                std::optional<std::string> testOutputDir = {};

            private:

                uint32_t width = 0;
                uint32_t height = 0;
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
                exqudens::vulkan::Queue graphicsQueue = {};
                exqudens::vulkan::Queue presentQueue = {};
                exqudens::vulkan::Swapchain swapchain = {};
                std::vector<exqudens::vulkan::ImageView> imageViews = {};
                exqudens::vulkan::RenderPass renderPass = {};
                exqudens::vulkan::PipelineLayout pipelineLayout = {};
                exqudens::vulkan::Pipeline pipeline = {};

            public:

                void run() {
                    init();
                    loop();
                    cleanup();
                }

            private:

                void init() {
                    glfwInit();

                    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

                    width = 800;
                    height = 600;
                    title = "Vulkan";
                    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

                    initVulkan();
                }

                void loop() {
                    while (!glfwWindowShouldClose(window)) {
                        glfwPollEvents();
                    }
                }

                void cleanup() {
                    glfwDestroyWindow(window);
                    glfwTerminate();
                }

                void initVulkan() {
                    std::vector<const char*> instanceRequiredExtensions = TestGlfwUtils::getRequiredInstanceExtensions();
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
                        .setPfnUserCallback(&VulkanTutorial1GuiTests::debugCallback)
                    )
                    .build(instance.target);

                    VkSurfaceKHR vkSurface = nullptr;
                    if (glfwCreateWindowSurface(*instance.target, window, nullptr, &vkSurface) != VK_SUCCESS || vkSurface == nullptr) {
                        throw std::runtime_error(CALL_INFO + ": failed to create surface!");
                    }

                    exqudens::vulkan::Surface::builder(surface)
                    .setSurface(vkSurface)
                    .build(instance.target);

                    std::optional<uint32_t> graphicsIndex = {};
                    std::optional<uint32_t> presentIndex = {};

                    std::vector<vk::raii::PhysicalDevice> devices = instance.target.enumeratePhysicalDevices();
                    for (auto& d : devices) {
                        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = d.getQueueFamilyProperties();
                        for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
                            if (
                                (queueFamilyProperties.at(i).queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0)
                                && d.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface.surface)
                            ) {
                                graphicsIndex = static_cast<uint32_t>(i);
                                presentIndex = graphicsIndex.value();
                                break;
                            }
                        }
                        std::set<std::string> notFoundExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());
                        std::vector<vk::ExtensionProperties> extensionProperties = d.enumerateDeviceExtensionProperties();
                        for (const auto& ep : extensionProperties) {
                            notFoundExtensions.erase(ep.extensionName);
                        }
                        if (graphicsIndex.has_value() && presentIndex.has_value() && notFoundExtensions.empty()) {
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

                    exqudens::vulkan::Queue::builder(graphicsQueue)
                    .setFamilyIndex(graphicsIndex.value())
                    .build(device.target);

                    exqudens::vulkan::Queue::builder(presentQueue)
                    .setFamilyIndex(presentIndex.value())
                    .build(device.target);

                    exqudens::vulkan::Swapchain::builder(swapchain)
                    .setCreateInfo(
                        exqudens::vulkan::Swapchain::createInfoFrom(
                            physicalDevice.target,
                            surface.target,
                            framebufferWidth,
                            framebufferHeight
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

                    imageViews.resize(swapchain.target.getImages().size());
                    for (size_t i = 0; i < imageViews.size(); i++) {
                        exqudens::vulkan::ImageView::builder(imageViews.at(i))
                        .setCreateInfo(
                            vk::ImageViewCreateInfo()
                            .setImage(swapchain.target.getImages().at(i))
                            .setFormat(swapchain.createInfo.imageFormat)
                            .setViewType(vk::ImageViewType::e2D)
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
                    }

                    exqudens::vulkan::RenderPass::builder(renderPass)
                    .addAttachmentDescription(
                        vk::AttachmentDescription()
                        .setFormat(swapchain.createInfo.imageFormat)
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
                    .addSubpassDescription(
                        vk::SubpassDescription()
                        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                    )
                    .setCreateInfo(
                        vk::RenderPassCreateInfo()
                    )
                    .build(device.target);

                    exqudens::vulkan::PipelineLayout::builder(pipelineLayout)
                    .setCreateInfo(
                        vk::PipelineLayoutCreateInfo()
                        .setSetLayoutCount(0)
                        .setPushConstantRangeCount(0)
                    )
                    .build(device.target);

                    exqudens::vulkan::ShaderModule vertShaderModule = {};
                    exqudens::vulkan::ShaderModule::builder(vertShaderModule)
                    .setFile(
                        std::filesystem::path(testOutputDir.value())
                        .append("vert.spv")
                        .generic_string()
                        .c_str()
                    )
                    .setReadFile(true)
                    .build(device.target);

                    exqudens::vulkan::ShaderModule fragShaderModule = {};
                    exqudens::vulkan::ShaderModule::builder(fragShaderModule)
                    .setFile(
                        std::filesystem::path(testOutputDir.value())
                        .append("frag.spv")
                        .generic_string()
                        .c_str()
                    )
                    .setReadFile(true)
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
                    .setVertexInputStateCreateInfo(
                        vk::PipelineVertexInputStateCreateInfo()
                        .setVertexBindingDescriptionCount(0)
                        .setVertexAttributeDescriptionCount(0)
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
                        .setDepthClampEnable(false)
                        .setRasterizerDiscardEnable(false)
                        .setPolygonMode(vk::PolygonMode::eFill)
                        .setLineWidth(1.0f)
                        .setCullMode(vk::CullModeFlagBits::eBack)
                        .setFrontFace(vk::FrontFace::eClockwise)
                        .setDepthBiasEnable(false)
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
                        .setPColorAttachmentFormats(&swapchain.createInfo.imageFormat)
                    )
                    .setGraphicsCreateInfo(
                        vk::GraphicsPipelineCreateInfo()
                        .setLayout(*pipelineLayout.target)
                        .setRenderPass(*renderPass.target)
                        .setSubpass(0)
                    )
                    .build(device.target);

                    vertShaderModule.target.clear();
                    vertShaderModule.target.release();
                    fragShaderModule.target.clear();
                    fragShaderModule.target.release();

                    EXQUDENS_LOG_INFO(LOGGER_ID) << "pipeline: " << (pipeline.target != nullptr);

                    // TODO
                }

        };

};

/*
    @brief docs.vulkan.org/tutorial/latest/Drawing_a_triangle/Setup/Base_code
*/
TEST_F(VulkanTutorial1GuiTests, test1) {
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
