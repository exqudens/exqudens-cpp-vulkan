#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <exqudens/Log.hpp>
#include <exqudens/log/api/Logging.hpp>

#include "TestUtils.hpp"
#include "StringVector.hpp"

class StringVectorUnitTests : public testing::Test {

    public:

        inline static const char* LOGGER_ID = "StringVectorUnitTests";

    protected:

        std::string toString(char** data, size_t i) {
            std::string result = data[i];
            return result;
        }

};

TEST_F(StringVectorUnitTests, test1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        StringVector data = {};
        std::vector<std::string> expected = {};
        std::vector<std::string> actual = {};

        // case-1
        expected = {"123"};
        for (size_t i = 0; i < expected.size(); i++) EXQUDENS_LOG_INFO(LOGGER_ID) << "expected[" << i << "]: '" << expected.at(i) << "'";
        data = expected;
        actual = data;
        for (size_t i = 0; i < actual.size(); i++) EXQUDENS_LOG_INFO(LOGGER_ID) << "actual[" << i << "]: '" << actual.at(i) << "'";

        ASSERT_TRUE(expected == actual);

        // case-2
        expected = {"234", "345"};
        for (size_t i = 0; i < expected.size(); i++) EXQUDENS_LOG_INFO(LOGGER_ID) << "expected[" << i << "]: '" << expected.at(i) << "'";
        data = expected;
        actual = data;
        for (size_t i = 0; i < actual.size(); i++) EXQUDENS_LOG_INFO(LOGGER_ID) << "actual[" << i << "]: '" << actual.at(i) << "'";

        ASSERT_TRUE(expected == actual);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

TEST_F(StringVectorUnitTests, test2) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        StringVector data = {};
        std::string expected = "";
        std::string actual = "";

        // case-1
        expected = "123";
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: '" << expected << "'";
        data = {expected};
        actual = toString(data.c_data(), 0);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: '" << actual << "'";

        ASSERT_EQ(expected, actual);

        // case-2
        expected = "234";
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: '" << expected << "'";
        data = {"123", expected, "345"};
        actual = toString(data.c_data(), 1);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: '" << actual << "'";

        ASSERT_EQ(expected, actual);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}
