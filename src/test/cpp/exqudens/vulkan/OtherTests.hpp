#pragma once

#include <iostream>
#include <format>

#include <gtest/gtest.h>

#include "TestUtils.hpp"

namespace exqudens::vulkan {

  class OtherTests : public testing::Test {};

  TEST_F(OtherTests, test1) {
    try {
      int expected = 123;
      int actual = 123;
      ASSERT_EQ(expected, actual);
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
