#include <cstddef>
#include <cstdlib>
#include <string>
#include <optional>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <exqudens/Log.hpp>

#include "TestApplication.hpp"
#include "TestUtils.hpp"

// include test files
#include "unit/OtherUnitTests.hpp"
#include "unit/GlmUnitTests.hpp"
#include "unit/StringVectorUnitTests.hpp"
#include "gui/vulkan-tutorial-com/VulkanTutorialCom1GuiTests.hpp"
#include "gui/vulkan-tutorial-com/VulkanTutorialCom2GuiTests.hpp"
#include "gui/vulkan-tutorial-com/VulkanTutorialCom3GuiTests.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

int TestApplication::run(int argc, char** argv) {
    try {
        std::vector<std::string> args = {};
        for (int i = 0; i < argc; i++) {
            std::cout << "argv[" << i << "]: '" << argv[i] << "'" << std::endl;
            args.emplace_back(argv[i]);
        }

        TestUtils::init(args);

        // logging
        std::filesystem::path executableDir(TestUtils::getExecutableDir().value());
        std::string loggingFile = (executableDir / "log" / "log.txt").generic_string();
        std::string loggingConfigJson = TestUtils::readFileString(
            std::filesystem::path(__FILE__)
            .parent_path()
            .parent_path()
            .append("resources")
            .append("log-config.json")
            .generic_string()
        );
        std::string loggingConfigAnchorFile = "@FILE@";
        size_t loggingConfigAnchorFilePosition = loggingConfigJson.find(loggingConfigAnchorFile);
        if (loggingConfigAnchorFilePosition != std::string::npos) {
            loggingConfigJson.replace(loggingConfigAnchorFilePosition, loggingConfigAnchorFile.length(), loggingFile);
        }
        std::string loggingConfigResult = exqudens::log::api::Logging::configure(loggingConfigJson);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";
        EXQUDENS_LOG_INFO(LOGGER_ID) << "loggingConfigResult: '" << loggingConfigResult << "'";

        testing::InitGoogleMock(&argc, argv);
        testing::InitGoogleTest(&argc, argv);

        int result = RUN_ALL_TESTS();

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";

        return result;
    } catch (const std::exception& e) {
        std::cerr << CALL_INFO + ": " + TestUtils::toString(e) << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << CALL_INFO + ": Unknown error!" << std::endl;
        return EXIT_FAILURE;
    }
}

#undef CALL_INFO
