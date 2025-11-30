#include <cstdlib>
#include <string>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestApplication.hpp"
#include "TestUtils.hpp"
#include "TestConfiguration.hpp"
#include "TestLogging.hpp"

// tests
#include "exqudens/vulkan/DrawingTriangleSystemTests.hpp"
#include "exqudens/vulkan/OtherTests.hpp"
#include "exqudens/vulkan/TestUtilsTests.hpp"
#include "exqudens/vulkan/TinyObjLoaderTests.hpp"
#include "exqudens/vulkan/Tests.hpp"
#include "exqudens/vulkan/UiTests.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

int TestApplication::run(int argc, char** argv) noexcept {
  try {
    if (argc < 1) {
      throw std::invalid_argument(CALL_INFO + ": 'argc' less than zero!");
    }

    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
      args.emplace_back(argv[i]);
    }

    TestConfiguration::setExecutableFile(args.front());

    std::filesystem::path dir = std::filesystem::path(args.front()).parent_path();

    if (!std::filesystem::exists(dir / "log")) {
      std::filesystem::create_directories(dir / "log");
    }

    std::string configType = TestLogging::config(args);

    TEST_LOG_I(LOGGER_ID) << "start config type: '" << configType << "'";

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

    TEST_LOG_I(LOGGER_ID) << "end";

    return result;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << TestUtils::toString(e) << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Error: Unknown cause!" << std::endl;
    return EXIT_FAILURE;
  }
}

#undef CALL_INFO
