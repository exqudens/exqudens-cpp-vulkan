#pragma once

#include <string>
#include <any>
#include <map>

#include <gtest/gtest.h>

#include "TestUtils.hpp"
#include "TestLogging.hpp"
#include "exqudens/vulkan/all.hpp"

namespace exqudens::vulkan {

  class OtherTests : public testing::Test {

    protected:

      inline static const char* LOGGER_ID = "exqudens.vulkan.OtherTests";

  };

  TEST_F(OtherTests, test1) {
    try {
      std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
      std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' start";

      int expected = 123;
      int actual = 123;

      TEST_LOG_I(LOGGER_ID) << "expected: '" << expected << "'";
      TEST_LOG_I(LOGGER_ID) << "actual: '" << actual << "'";

      ASSERT_EQ(expected, actual);

      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' end";
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
