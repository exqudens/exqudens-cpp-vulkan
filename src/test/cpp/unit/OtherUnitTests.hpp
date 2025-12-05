#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <exqudens/Log.hpp>
#include <exqudens/log/api/Logging.hpp>
#include <nlohmann/json.hpp>

// vulkan
#include <vulkan/vulkan.hpp>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "TestUtils.hpp"

class OtherUnitTests: public testing::Test {

    public:

        inline static const char* LOGGER_ID = "OtherUnitTests";

    protected:

        static void log(
            const std::string& file,
            const size_t& line,
            const std::string& function,
            const std::string& id,
            const unsigned short& level,
            const std::string& message
        ) {
            exqudens::log::api::Logging::Writer(file, line, function, id, level) << message;
        }

};

TEST_F(OtherUnitTests, test1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        std::filesystem::path projectSourceDir = {};
        std::string nameVersionContent = {};
        nlohmann::json nameVersionJosn = {};
        std::string expected = {};
        std::string actual = {};

        projectSourceDir = std::filesystem::path(TestUtils::getProjectSourceDir());
        nameVersionContent = TestUtils::readFileString((projectSourceDir / "name-version.json").generic_string());
        nameVersionJosn = nlohmann::json::parse(nameVersionContent);

        expected = nameVersionJosn.at("version");
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: '" << expected << "'";

        actual = "0.0.1";
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: '" << actual << "'";

        ASSERT_EQ(expected, actual);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}
