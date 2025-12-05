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

class VulkanTutorial2GuiTests: public testing::Test {

    public:

        inline static const char* LOGGER_ID = "VulkanTutorial2GuiTests";

    protected:

        struct Data {
            exqudens::vulkan::Context context = {};
            exqudens::vulkan::Instance instance = {};
        };

        class Application {

            private:

                uint32_t width = 0;
                uint32_t height = 0;
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
                    data.context = exqudens::vulkan::Context::builder()
                        .build();
                    data.instance = exqudens::vulkan::Instance::builder()
                        .addEnabledExtensionNames(TestGlfwUtils::getRequiredInstanceExtensions())
                        .setContext(data.context.getPtr())
                        .build();
                }

        };

};

/*
    @brief docs.vulkan.org/tutorial/latest/Drawing_a_triangle/Setup/Instance
*/
TEST_F(VulkanTutorial2GuiTests, test1) {
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
