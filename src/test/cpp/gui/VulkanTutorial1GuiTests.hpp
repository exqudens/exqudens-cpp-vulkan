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
                std::vector<exqudens::vulkan::Framebuffer> framebuffers = {};
                exqudens::vulkan::CommandPool commandPool = {};
                exqudens::vulkan::CommandBuffers commandBuffers = {};

                std::vector<exqudens::vulkan::Semaphore> imageAvailableSemaphores = {};
                std::vector<exqudens::vulkan::Semaphore> renderFinishedSemaphores = {};
                std::vector<exqudens::vulkan::Fence> inFlightFences = {};

                uint32_t currentFrame = 0;

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
                        drawFrame();
                    }

                    device.target.waitIdle();
                }

                void cleanup() {
                    glfwDestroyWindow(window);
                    glfwTerminate();
                }

                void initVulkan() {
                    EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

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

                    framebuffers.resize(swapchain.target.getImages().size());
                    for (size_t i = 0; i < framebuffers.size(); i++) {
                        exqudens::vulkan::Framebuffer::builder(framebuffers.at(i))
                        .addAttachment(*imageViews.at(i).target)
                        .setCreateInfo(
                            vk::FramebufferCreateInfo()
                            .setRenderPass(*renderPass.target)
                            .setWidth(swapchain.createInfo.imageExtent.width)
                            .setHeight(swapchain.createInfo.imageExtent.height)
                            .setLayers(1)
                        )
                        .build(device.target);
                    }

                    exqudens::vulkan::CommandPool::builder(commandPool)
                    .setCreateInfo(
                        vk::CommandPoolCreateInfo()
                        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                        .setQueueFamilyIndex(graphicsQueue.familyIndex.value())
                    )
                    .build(device.target);

                    exqudens::vulkan::CommandBuffers::builder(commandBuffers)
                    .setAllocateInfo(
                        vk::CommandBufferAllocateInfo()
                        .setCommandPool(*commandPool.target)
                        .setLevel(vk::CommandBufferLevel::ePrimary)
                        .setCommandBufferCount(static_cast<uint32_t>(swapchain.target.getImages().size()))
                    )
                    .build(device.target);

                    imageAvailableSemaphores.resize(swapchain.target.getImages().size());
                    for (size_t i = 0; i < imageAvailableSemaphores.size(); i++) {
                        exqudens::vulkan::Semaphore::builder(imageAvailableSemaphores.at(i)).build(device.target);
                    }

                    renderFinishedSemaphores.resize(swapchain.target.getImages().size());
                    for (size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
                        exqudens::vulkan::Semaphore::builder(renderFinishedSemaphores.at(i)).build(device.target);
                    }

                    inFlightFences.resize(swapchain.target.getImages().size());
                    for (size_t i = 0; i < inFlightFences.size(); i++) {
                        exqudens::vulkan::Fence::builder(inFlightFences.at(i)).setCreateInfo(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled)).build(device.target);
                    }

                    EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
                }

                void recordCommandBuffer(vk::raii::CommandBuffer& commandBuffer, size_t imageIndex) {
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
                            .setExtent(swapchain.createInfo.imageExtent)
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
                        .setWidth(static_cast<float>(swapchain.createInfo.imageExtent.width))
                        .setHeight(static_cast<float>(swapchain.createInfo.imageExtent.height))
                        .setMinDepth(0.0f)
                        .setMaxDepth(1.0f)
                    });

                    commandBuffer.setScissor(0, {
                        vk::Rect2D()
                        .setOffset(vk::Offset2D().setY(0).setY(0))
                        .setExtent(swapchain.createInfo.imageExtent)
                    });

                    commandBuffer.draw(3, 1, 0, 0);

                    commandBuffer.endRenderPass();
                    commandBuffer.end();
                }

                void drawFrame() {
                    (void) device.target.waitForFences({*inFlightFences.at(currentFrame).target}, true, UINT64_MAX);
                    device.target.resetFences({*inFlightFences.at(currentFrame).target});

                    uint32_t imageIndex = swapchain.target.acquireNextImage(UINT64_MAX, *imageAvailableSemaphores.at(currentFrame).target, nullptr).value;

                    commandBuffers.targets.at(currentFrame).reset();
                    recordCommandBuffer(commandBuffers.targets.at(currentFrame), imageIndex);

                    std::vector<vk::Semaphore> waitSemaphores = {
                        *imageAvailableSemaphores.at(currentFrame).target
                    };
                    std::vector<vk::PipelineStageFlags> waitStages = {
                        vk::PipelineStageFlagBits::eColorAttachmentOutput
                    };
                    std::vector<vk::CommandBuffer> waitCommandBuffers = {
                        *commandBuffers.targets.at(currentFrame)
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

                    (void) presentQueue.target.presentKHR(
                        vk::PresentInfoKHR()
                        .setWaitSemaphores(signalSemaphores)
                        .setSwapchains(swapchains)
                        .setPImageIndices(&imageIndex)
                    );

                    currentFrame = (currentFrame + 1) % static_cast<uint32_t>(swapchain.target.getImages().size());
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
