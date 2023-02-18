#pragma once

#include <string>
#include <any>
#include <map>

#include <gtest/gtest.h>

#include "TestUtils.hpp"
#include "exqudens/vulkan/all.hpp"

namespace exqudens::vulkan {

  class OtherTests : public testing::Test {};

  TEST_F(OtherTests, test1) {
    try {
      int expected = 123;
      int actual = 123;

      PRINTLN(expected);
      PRINTLN(actual);

      ASSERT_EQ(expected, actual);
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
