#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <stdexcept>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <iostream>

#include <gtest/gtest.h>
#include <exqudens/Log.hpp>
#include <exqudens/log/api/Logging.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "TestUtils.hpp"
#include "TestGlfwUtils.hpp"
#include "exqudens/vulkan.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

class VulkanTutorial4GuiTests: public testing::Test {

    public:

        inline static const char* LOGGER_ID = "VulkanTutorial4GuiTests";

        static VKAPI_ATTR VULKAN_HPP_NAMESPACE::Bool32 VKAPI_CALL debugCallback(
            VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT severity,
            VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagsEXT type,
            const VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        ) {
            if (severity >= VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
                if (severity == VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
                    EXQUDENS_LOG_TRACE(LOGGER_ID) << pCallbackData->pMessage;
                } else if (severity == VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) {
                    EXQUDENS_LOG_INFO(LOGGER_ID) << pCallbackData->pMessage;
                } else if (severity == VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
                    EXQUDENS_LOG_WARNING(LOGGER_ID) << pCallbackData->pMessage;
                } else if (severity == VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
                    EXQUDENS_LOG_ERROR(LOGGER_ID) << pCallbackData->pMessage;
                }
            }
            return VULKAN_HPP_NAMESPACE::False;
        }

        static bool physicalDeviceFilter(
            size_t index,
            const VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& device,
            std::vector<const char*> requiredExtensions
        ) {
            bool supportsGraphics = false;
            //bool discreteGpu = false;
            //bool supportsVulkan1_4 = false;
            bool supportsRequiredFeatures = false;
            bool supportsRequiredExtensions = false;

            //discreteGpu = device.getProperties().deviceType == VULKAN_HPP_NAMESPACE::PhysicalDeviceType::eDiscreteGpu;

            //supportsVulkan1_4 = device.getProperties().apiVersion >= VULKAN_HPP_NAMESPACE::ApiVersion14;

            std::vector<VULKAN_HPP_NAMESPACE::QueueFamilyProperties> queueFamilyProperties = {};
            queueFamilyProperties = device.getQueueFamilyProperties();
            for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
                VULKAN_HPP_NAMESPACE::QueueFamilyProperties qfp = queueFamilyProperties.at(i);
                if ((qfp.queueFlags & VULKAN_HPP_NAMESPACE::QueueFlagBits::eGraphics) != static_cast<VULKAN_HPP_NAMESPACE::QueueFlags>(0)) {
                    supportsGraphics = true;
                    break;
                }
            }

            /*auto features = device.getFeatures2<
                VULKAN_HPP_NAMESPACE::PhysicalDeviceFeatures2,
                VULKAN_HPP_NAMESPACE::PhysicalDeviceVulkan13Features,
                VULKAN_HPP_NAMESPACE::PhysicalDeviceExtendedDynamicStateFeaturesEXT
            >();
            supportsRequiredFeatures = features.get<VULKAN_HPP_NAMESPACE::PhysicalDeviceVulkan13Features>().dynamicRendering
            && features.get<VULKAN_HPP_NAMESPACE::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;*/
            auto features = device.getFeatures2<
                VULKAN_HPP_NAMESPACE::PhysicalDeviceFeatures2,
                VULKAN_HPP_NAMESPACE::PhysicalDeviceHostQueryResetFeatures
            >();
            supportsRequiredFeatures = features.get<VULKAN_HPP_NAMESPACE::PhysicalDeviceFeatures2>().features.samplerAnisotropy
            && features.get<VULKAN_HPP_NAMESPACE::PhysicalDeviceFeatures2>().features.sampleRateShading
            && features.get<VULKAN_HPP_NAMESPACE::PhysicalDeviceHostQueryResetFeatures>().hostQueryReset;

            std::set<std::string> requiredExtensionsSet(requiredExtensions.begin(), requiredExtensions.end());
            std::vector<VULKAN_HPP_NAMESPACE::ExtensionProperties> extensionProperties = {};
            extensionProperties = device.enumerateDeviceExtensionProperties();
            std::set<std::string> availableExtensionsSet = {};
            for (const auto& ep : extensionProperties) {
                std::string extensionName = ep.extensionName;
                availableExtensionsSet.insert(extensionName);
            }
            supportsRequiredExtensions = std::includes(
                availableExtensionsSet.begin(), availableExtensionsSet.end(),
                requiredExtensionsSet.begin(), requiredExtensionsSet.end()
            );

            return supportsGraphics
            //&& discreteGpu
            //&& supportsVulkan1_4
            && supportsRequiredFeatures
            && supportsRequiredExtensions;
        }

    protected:

        class Application {

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

                VULKAN_HPP_NAMESPACE::raii::Context context;
                exqudens::vulkan::Instance instance = {};
                exqudens::vulkan::DebugUtilsMessenger debugUtilsMessenger = {};
                exqudens::vulkan::PhysicalDevice physicalDevice = {};

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
                    std::vector<const char*> requiredExtensions = TestGlfwUtils::getRequiredInstanceExtensions();
                    std::vector<const char*> requiredLayers = {};

                    if (enableValidationLayers) {
                        requiredExtensions.emplace_back(VULKAN_HPP_NAMESPACE::EXTDebugUtilsExtensionName);
                        requiredLayers.emplace_back("VK_LAYER_KHRONOS_validation");
                    }

                    exqudens::vulkan::Instance::builder()
                    .setApplicationInfo(
                        VULKAN_HPP_NAMESPACE::ApplicationInfo()
                        .setApiVersion(VULKAN_HPP_NAMESPACE::ApiVersion14)
                        .setPApplicationName(LOGGER_ID)
                        .setApplicationVersion(VK_MAKE_VERSION(0, 0, 1))
                        .setPEngineName("No Engine")
                        .setEngineVersion(VK_MAKE_VERSION(0, 0, 1))
                    )
                    .setEnabledExtensionNames(requiredExtensions)
                    .setEnabledLayerNames(requiredLayers)
                    .build(instance, context);

                    exqudens::vulkan::DebugUtilsMessenger::builder()
                    .setCreateInfo(
                        VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCreateInfoEXT()
                        .setMessageSeverity(
                            VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
                            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eError
                        )
                        .setMessageType(
                            VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                            | VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                            //| VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
                        )
                        .setPfnUserCallback(&VulkanTutorial4GuiTests::debugCallback)
                    )
                    .build(debugUtilsMessenger, instance.target);

                    exqudens::vulkan::PhysicalDevice::builder()
                    .setRequiredExtensions({
                        //VULKAN_HPP_NAMESPACE::KHRSpirv14ExtensionName,
                        //VULKAN_HPP_NAMESPACE::KHRSynchronization2ExtensionName,
                        //VULKAN_HPP_NAMESPACE::KHRCreateRenderpass2ExtensionName,
                        VULKAN_HPP_NAMESPACE::KHRSwapchainExtensionName,
                        VULKAN_HPP_NAMESPACE::EXTHostQueryResetExtensionName
                    })
                    .setFilterFunction(&VulkanTutorial4GuiTests::physicalDeviceFilter)
                    .build(physicalDevice, instance.target);

                    EXQUDENS_LOG_INFO(LOGGER_ID) << "physicalDevice: " << (physicalDevice.target != nullptr);
                }

        };

};

/*
    @brief docs.vulkan.org/tutorial/latest/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
*/
TEST_F(VulkanTutorial4GuiTests, test1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        Application app;

        app.run();

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

#undef CALL_INFO
