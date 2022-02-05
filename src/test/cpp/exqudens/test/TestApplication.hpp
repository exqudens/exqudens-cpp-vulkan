#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "exqudens/test/TestConfiguration.hpp"
#include "exqudens/test/Tests.hpp"

namespace exqudens::vulkan {

  class TestApplication {

    public:

      static int run(int* argc, char** argv) {
        TestConfiguration::setExecutableFile(argv[0]);
        testing::InitGoogleMock(argc, argv);
        testing::InitGoogleTest(argc, argv);
        return RUN_ALL_TESTS();
      }

  };

}