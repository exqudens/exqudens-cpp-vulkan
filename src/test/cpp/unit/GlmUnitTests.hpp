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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "TestUtils.hpp"

class GlmUnitTests: public testing::Test {

    public:

        inline static const char* LOGGER_ID = "GlmUnitTests";

};

/*
*    @brief The Identity matrix
*    links:
*    - https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices
*/
TEST_F(GlmUnitTests, test_identityMatrix_1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        float epsilon = 0.0001f;
        glm::vec2 inPosition = {0.5f, 0.5f};

        glm::mat4 proj = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec4 expected = glm::vec4(inPosition, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: " << glm::to_string(expected);

        glm::vec4 actual = proj * view * model * glm::vec4(inPosition, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: " << glm::to_string(actual);

        ASSERT_TRUE(glm::all(glm::epsilonEqual(expected, actual, epsilon)));

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

/*
*    @brief Rotation matrices
*    links:
*    - https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices
*/
TEST_F(GlmUnitTests, test_rotate_1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        float epsilon = 0.0001f;
        glm::vec2 inPosition = {0.5f, 0.5f};

        glm::mat4 proj = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), 1.0f * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::vec4 expected = glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: " << glm::to_string(expected);

        glm::vec4 actual = proj * view * model * glm::vec4(inPosition, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: " << glm::to_string(actual);

        ASSERT_TRUE(glm::all(glm::epsilonEqual(expected, actual, epsilon)));

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

/*
*    @brief Rotation matrices
*    links:
*    - https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices
*/
TEST_F(GlmUnitTests, test_lookAt_1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        float epsilon = 0.0001f;
        glm::vec2 inPosition = {0.5f, 0.5f};

        glm::mat4 proj = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec4 expected = glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: " << glm::to_string(expected);

        glm::vec4 actual = proj * view * model * glm::vec4(inPosition, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: " << glm::to_string(actual);

        //ASSERT_TRUE(glm::all(glm::epsilonEqual(expected, actual, epsilon)));

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

/*
*    @brief Rotation matrices
*    links:
*    - https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices
*/
TEST_F(GlmUnitTests, test_perspective_1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        float epsilon = 0.0001f;
        glm::vec2 inPosition = {0.5f, 0.5f};

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), static_cast<float>(800) / static_cast<float>(600), 0.1f, 10.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec4 expected = glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: " << glm::to_string(expected);

        glm::vec4 actual = proj * view * model * glm::vec4(inPosition, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: " << glm::to_string(actual);

        //ASSERT_TRUE(glm::all(glm::epsilonEqual(expected, actual, epsilon)));

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

/*
*    @brief Rotation matrices
*    links:
*    - https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices
*/
TEST_F(GlmUnitTests, test_rotate_lookAt_perspective_1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        float epsilon = 0.0001f;
        glm::vec2 inPosition = {0.5f, 0.5f};

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), static_cast<float>(800) / static_cast<float>(600), 0.1f, 10.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), 1.0f * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::vec4 expected = glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: " << glm::to_string(expected);

        glm::vec4 actual = proj * view * model * glm::vec4(inPosition, 0.0f, 1.0f);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: " << glm::to_string(actual);

        //ASSERT_TRUE(glm::all(glm::epsilonEqual(expected, actual, epsilon)));

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}
