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
#include "exqudens/vulkan/Context.hpp"
#include "exqudens/vulkan/Instance.hpp"
#include "exqudens/vulkan/PhysicalDevice.hpp"

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
            VULKAN_HPP_NAMESPACE::raii::PhysicalDevice device,
            std::vector<const char*> requiredExtensions
        ) {
            bool discreteGpu = false;
            bool supportsVulkan1_4 = false;
            bool supportsGraphics = false;
            bool supportsRequiredExtensions = false;
            bool supportsRequiredFeatures = false;

            discreteGpu = device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu;

            supportsVulkan1_4 = device.getProperties().apiVersion >= VULKAN_HPP_NAMESPACE::ApiVersion14;

            std::vector<VULKAN_HPP_NAMESPACE::QueueFamilyProperties> queueFamilyProperties = {};
            queueFamilyProperties = device.getQueueFamilyProperties();
            for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
                VULKAN_HPP_NAMESPACE::QueueFamilyProperties qfp = queueFamilyProperties.at(i);
                if ((qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0)) {
                    supportsGraphics = true;
                    break;
                }
            }

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

            auto features = device.getFeatures2<
                vk::PhysicalDeviceFeatures2,
                vk::PhysicalDeviceVulkan13Features,
                vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
            >();
            supportsRequiredFeatures = features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering
            && features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

            return discreteGpu && supportsVulkan1_4 && supportsGraphics && supportsRequiredExtensions && supportsRequiredFeatures;
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

                exqudens::vulkan::Context context = {};
                exqudens::vulkan::Instance instance = {};
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

                    context = {};

                    instance = exqudens::vulkan::Instance::builder()
                    .setApiVersion(VULKAN_HPP_NAMESPACE::ApiVersion14)
                    .setApplicationName(LOGGER_ID)
                    .setApplicationVersion(VK_MAKE_VERSION(0, 0, 1))
                    .setEngineName("No Engine")
                    .setEngineVersion(VK_MAKE_VERSION(0, 0, 1))
                    .setEnabledExtensionNames(requiredExtensions)
                    .setEnabledLayerNames(requiredLayers)
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
                    .setDebugCallback(&VulkanTutorial4GuiTests::debugCallback)
                    .build(std::ref(context.get()));

                    physicalDevice = exqudens::vulkan::PhysicalDevice::builder()
                    .setRequiredExtensions({
                        VULKAN_HPP_NAMESPACE::KHRSwapchainExtensionName,
                        VULKAN_HPP_NAMESPACE::KHRSpirv14ExtensionName,
                        VULKAN_HPP_NAMESPACE::KHRSynchronization2ExtensionName,
                        VULKAN_HPP_NAMESPACE::KHRCreateRenderpass2ExtensionName
                    })
                    .setFilterFunction(&VulkanTutorial4GuiTests::physicalDeviceFilter)
                    .build(instance.get());

                    EXQUDENS_LOG_INFO(LOGGER_ID) << "physical device: " << (physicalDevice.get() == nullptr);
                }

        };

};

/*
    @brief docs.vulkan.org/tutorial/latest/Drawing_a_triangle/Setup/Validation_layers
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
