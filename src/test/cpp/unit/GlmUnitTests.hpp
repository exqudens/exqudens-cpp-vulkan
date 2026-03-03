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

// glm
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "TestUtils.hpp"

class GlmUnitTests: public testing::Test {

    public:

        inline static const char* LOGGER_ID = "GlmUnitTests";

};

TEST_F(GlmUnitTests, test1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        glm::vec2 inPosition = {0.5f, 0.5f};

        glm::mat4 proj = {1.0f};
        glm::mat4 view = {1.0f};
        glm::mat4 model = {1.0f};

        glm::vec4 expected = glm::vec4(inPosition, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: " << glm::to_string(expected);

        glm::vec4 actual = proj * view * model * glm::vec4(inPosition, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: " << glm::to_string(actual);

        ASSERT_TRUE(expected == actual);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}
