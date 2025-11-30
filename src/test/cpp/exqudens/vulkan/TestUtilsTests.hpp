#pragma once

#include <string>
#include <sstream>
#include <filesystem>
#include <stdexcept>

#include <gtest/gtest.h>

#include "TestUtils.hpp"
#include "TestLogging.hpp"
#include "TestConfiguration.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  class TestUtilsTests : public testing::Test {

    protected:

      inline static const char* LOGGER_ID = "exqudens.vulkan.TestUtilsTests";

      void func3() {
        try {
          throw std::invalid_argument(CALL_INFO + ": Test error message!");
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
      }

      void func2() {
        try {
          func3();
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
      }

      void func1() {
        try {
          func2();
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
      }

  };

  TEST_F(TestUtilsTests, test1) {
    try {
      std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
      std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' start";

      uint64_t a = 123;
      int64_t b = -123;
      float_t c = 123.123f;

      uint64_t x = 123;
      int64_t y = -123;
      float_t z = 123.123f;

      size_t expected1 = 0;
      size_t actual1 = 0;

      TestUtils::hashCombine(expected1, a);
      TestUtils::hashCombine(actual1, x);

      std::cout << "expected1: '{}'" << expected1 << "'" << std::endl;
      std::cout << "  actual1: '{}'" << actual1 << "'" << std::endl;

      ASSERT_EQ(expected1, actual1);

      size_t expected2 = 0;
      size_t actual2 = 0;

      TestUtils::hashCombine(expected2, a, b);
      TestUtils::hashCombine(actual2, x, y);

      std::cout << "expected2: '{}'" << expected2 << "'" << std::endl;
      std::cout << "  actual2: '{}'" << actual2 << "'" << std::endl;

      ASSERT_EQ(expected2, actual2);

      size_t expected3 = 0;
      size_t actual3 = 0;

      TestUtils::hashCombine(expected3, a, b, c);
      TestUtils::hashCombine(actual3, x, y, z);

      std::cout << "expected3: '{}'" << expected3 << "'" << std::endl;
      std::cout << "  actual3: '{}'" << actual3 << "'" << std::endl;

      ASSERT_EQ(expected3, actual3);

      ASSERT_NE(actual1, actual2);
      ASSERT_NE(actual1, actual3);
      ASSERT_NE(actual2, actual3);

      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' end";
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

  TEST_F(TestUtilsTests, test2) {
    try {
      std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
      std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' start";

      RecordProperty("TestUtilsTests.test1.info1", "toString(const std::exception& e)");
      std::string thisFilePath = std::filesystem::path(__FILE__).make_preferred().string();
      std::ostringstream out;
      out << "exqudens::vulkan::TestUtilsTests::func3(" + thisFilePath + ":22): Test error message!" << std::endl;
      out << "exqudens::vulkan::TestUtilsTests::func3(" + thisFilePath + ":24)" << std::endl;
      out << "exqudens::vulkan::TestUtilsTests::func2(" + thisFilePath + ":32)" << std::endl;
      out << "exqudens::vulkan::TestUtilsTests::func1(" + thisFilePath + ":40)";
      std::string expected = out.str();
      try {
        func1();
        FAIL() << "(const std::exception& e) not thrown!";
      } catch (const std::exception& e) {
        std::string result = TestUtils::toString(e);
        ASSERT_EQ(expected, result);
      }

      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' end";
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

  TEST_F(TestUtilsTests, test3) {
    try {
      std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
      std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' start";

      RecordProperty("TestUtilsTests.test2.info1", "readPng(const std::string& path)");
      RecordProperty(
          "TestUtilsTests.test2.info2",
          "writePng(const std::vector<std::vector<std::vector<unsigned char>>>& image, const std::string& path)"
      );
      std::cout << "executableFile: '{}'" << TestConfiguration::getExecutableFile() << "'" << std::endl;
      std::cout << "executableDir: '{}'" << TestConfiguration::getExecutableDir() << "'" << std::endl;
      std::vector<std::vector<std::vector<unsigned char>>> image1 = TestUtils::readPng(
          std::filesystem::path(TestConfiguration::getExecutableDir())
              .append("resources")
              .append("png")
              .append("texture.png")
              .make_preferred()
              .string()
      );
      ASSERT_EQ(480, image1.size());
      ASSERT_EQ(640, image1[0].size());
      ASSERT_EQ(4, image1[0][0].size());
      std::cout << "image1.size OK!" << std::endl;
      TestUtils::writePng(
          image1,
          std::filesystem::path(TestConfiguration::getExecutableDir())
              .append("resources")
              .append("png")
              .append("texture1.png")
              .make_preferred()
              .string()
      );
      std::vector<std::vector<std::vector<unsigned char>>> image2 = TestUtils::readPng(
          std::filesystem::path(TestConfiguration::getExecutableDir())
              .append("resources")
              .append("png")
              .append("texture1.png")
              .make_preferred()
              .string()
      );
      ASSERT_EQ(480, image2.size());
      ASSERT_EQ(640, image2[0].size());
      ASSERT_EQ(4, image2[0][0].size());
      std::cout << "image2.size OK!" << std::endl;
      ASSERT_EQ(image1, image2);

      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' end";
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}

#undef CALL_INFO
