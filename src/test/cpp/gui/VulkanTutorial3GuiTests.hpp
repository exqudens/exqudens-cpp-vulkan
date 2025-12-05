#pragma once

#include <cstdlib>
#include <cstdint>
#include <vector>
#include <memory>
#include <exception>
#include <iostream>

#include <gtest/gtest.h>
#include <exqudens/Log.hpp>
#include <exqudens/log/api/Logging.hpp>

#include "TestUtils.hpp"
#include "TestGlfwUtils.hpp"
#include "exqudens/vulkan/Context.hpp"
#include "exqudens/vulkan/Instance.hpp"

class VulkanTutorial3GuiTests: public testing::Test {

    public:

        inline static const char* LOGGER_ID = "VulkanTutorial3GuiTests";

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

    protected:

        struct Data {
            exqudens::vulkan::Context context = {};
            exqudens::vulkan::Instance instance = {};
        };

        class Application {

            private:

                uint32_t width = 0;
                uint32_t height = 0;
#ifdef NDEBUG
                bool enableValidationLayers = false;
#else
                bool enableValidationLayers = true;
#endif
                GLFWwindow* window = nullptr;
                Data data = {};

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
                    window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);

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
                    std::vector<const char*> requiredInstanceExtensions = TestGlfwUtils::getRequiredInstanceExtensions();
                    std::vector<const char*> requiredLayers = {};

                    if (enableValidationLayers) {
                        requiredInstanceExtensions.emplace_back(VULKAN_HPP_NAMESPACE::EXTDebugUtilsExtensionName);
                        requiredLayers.emplace_back("VK_LAYER_KHRONOS_validation");
                    }

                    data.context = exqudens::vulkan::Context::builder()
                        .build();
                    data.instance = exqudens::vulkan::Instance::builder()
                        .addEnabledExtensionNames(requiredInstanceExtensions)
                        .addEnabledLayerNames(requiredLayers)
                        .setDebugCallback(&VulkanTutorial3GuiTests::debugCallback)
                        .setContext(data.context.getPtr())
                        .build();

                    data.instance.getRaii().submitDebugUtilsMessageEXT(
                        VULKAN_HPP_NAMESPACE::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
                        VULKAN_HPP_NAMESPACE::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
                        VULKAN_HPP_NAMESPACE::DebugUtilsMessengerCallbackDataEXT().setPMessage("instance build done")
                    );
                }

        };

};

/*
    @brief docs.vulkan.org/tutorial/latest/Drawing_a_triangle/Setup/Validation_layers
*/
TEST_F(VulkanTutorial3GuiTests, test1) {
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
