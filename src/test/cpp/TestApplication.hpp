#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestConfiguration.hpp"
#include "exqudens/vulkan/OtherTests.hpp"
#include "exqudens/vulkan/TestUtilsTests.hpp"
#include "exqudens/vulkan/TinyObjLoaderTests.hpp"
#include "exqudens/vulkan/Tests.hpp"
#include "exqudens/vulkan/UiTests.hpp"
#include "exqudens/vulkan/Ui1Tests.hpp"
#include "exqudens/vulkan/Ui2Tests.hpp"

class TestApplication {

  public:

    static int run(int* argc, char** argv) {
      TestConfiguration::setExecutableFile(argv[0]);
      testing::InitGoogleMock(argc, argv);
      testing::InitGoogleTest(argc, argv);
      return RUN_ALL_TESTS();
    }

};
