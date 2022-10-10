#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestConfiguration.hpp"
#include "exqudens/vulkan/TestUtilsTests.hpp"
#include "exqudens/vulkan/TinyObjLoaderTests.hpp"
#include "exqudens/vulkan/OtherTests.hpp"
//#include "exqudens/vulkan/ConfigurationTests.hpp"
//#include "exqudens/vulkan/ShaderTests.hpp"
//#include "exqudens/vulkan/FactoryTests.hpp"
#include "exqudens/vulkan/UiTestsA.hpp"
//#include "exqudens/vulkan/UiTestsB.hpp"
//#include "exqudens/vulkan/UiTestsC.hpp"
//#include "exqudens/vulkan/UiTestsD.hpp"
//#include "exqudens/vulkan/UiTestsE.hpp"

class TestApplication {

  public:

    static int run(int* argc, char** argv) {
      TestConfiguration::setExecutableFile(argv[0]);
      testing::InitGoogleMock(argc, argv);
      testing::InitGoogleTest(argc, argv);
      return RUN_ALL_TESTS();
    }

};
