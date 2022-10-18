#pragma once

#include <string>
#include <any>
#include <array>
#include <utility>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <iostream>
#include <format>

#include <gtest/gtest.h>
#include <vulkan/vulkan_raii.hpp>

#include "TestUtils.hpp"

namespace exqudens::vulkan {

  class OtherTests : public testing::Test {};

  TEST_F(OtherTests, test1) {
    try {
      std::vector<std::pair<vk::StructureType, std::any>> features2;

      features2.emplace_back(
          std::make_pair(
              vk::PhysicalDeviceFeatures2::structureType,
              vk::PhysicalDeviceFeatures2().setFeatures(vk::PhysicalDeviceFeatures().setSamplerAnisotropy(true))
          )
      );

      features2.emplace_back(
          std::make_pair(
              vk::PhysicalDeviceHostQueryResetFeatures::structureType,
              vk::PhysicalDeviceHostQueryResetFeatures().setHostQueryReset(true)
          )
      );

      std::any_cast<vk::PhysicalDeviceFeatures2>(features2[0].second).setPNext(&features2[1].second);

      ASSERT_EQ(2, features2.size());

      ASSERT_EQ(vk::PhysicalDeviceFeatures2::structureType, features2[0].first);
      ASSERT_EQ(vk::PhysicalDeviceHostQueryResetFeatures::structureType, features2[1].first);

      ASSERT_EQ(true, std::any_cast<vk::PhysicalDeviceFeatures2>(features2[0].second).features.samplerAnisotropy);
      ASSERT_TRUE(std::any_cast<vk::PhysicalDeviceFeatures2>(features2[0].second).pNext == nullptr);
      ASSERT_EQ(true, std::any_cast<vk::PhysicalDeviceHostQueryResetFeatures>(features2[1].second).hostQueryReset);

      std::vector<std::pair<vk::StructureType, std::shared_ptr<void>>> fs2;

      fs2.emplace_back(
          std::make_pair(
              vk::PhysicalDeviceHostQueryResetFeatures::structureType,
              std::make_shared<vk::PhysicalDeviceHostQueryResetFeatures>(vk::PhysicalDeviceHostQueryResetFeatures().setHostQueryReset(true))
          )
      );

      fs2.emplace_back(
          std::make_pair(
              vk::PhysicalDeviceFeatures2::structureType,
              std::make_shared<vk::PhysicalDeviceFeatures2>(
                  vk::PhysicalDeviceFeatures2().setFeatures(vk::PhysicalDeviceFeatures().setSamplerAnisotropy(true)).setPNext(fs2[0].second.get())
              )
          )
      );

      //a->setPNext(a.get());
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
