#pragma once

#include <string>
#include <any>
#include <map>
#include <iostream>
#include <format>

#include <gtest/gtest.h>

#include "TestUtils.hpp"
#include "exqudens/vulkan/all.hpp"

namespace exqudens::vulkan {

  class OtherTests : public testing::Test {};

  TEST_F(OtherTests, test1) {
    try {
      int expected = 123;
      int actual = 123;

      std::cout << std::format("expected: '{}'", expected) << std::endl;
      std::cout << std::format("actual: '{}'", actual) << std::endl;

      ASSERT_EQ(expected, actual);
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
