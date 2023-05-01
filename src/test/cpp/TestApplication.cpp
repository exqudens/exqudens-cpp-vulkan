#include "TestApplication.hpp"
#include "TestConfiguration.hpp"
#include "exqudens/vulkan/OtherTests.hpp"
#include "exqudens/vulkan/TestUtilsTests.hpp"
#include "exqudens/vulkan/TinyObjLoaderTests.hpp"
#include "exqudens/vulkan/Tests.hpp"
#include "exqudens/vulkan/UiTests.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdlib>
#include <string>
#include <utility>
#include <iostream>

int TestApplication::run(int* argc, char** argv) {
  std::pair<int, std::string> result = TestConfiguration::setExecutableFile(argv[0]);
  int status = result.first;
  std::string error = result.second;
  if (status != EXIT_SUCCESS) {
    std::cerr << error << std::endl;
    return status;
  }
  testing::InitGoogleMock(argc, argv);
  testing::InitGoogleTest(argc, argv);
  status = RUN_ALL_TESTS();
  return status;
}
