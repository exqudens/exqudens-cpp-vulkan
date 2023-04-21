#include "TestConfiguration.hpp"
#include "TestMacros.hpp"
#include "TestUtils.hpp"

#include <cstdlib>
#include <stdexcept>
#include <filesystem>

std::string TestConfiguration::getExecutableFile() {
  try {
    return executableFile.value_or("");
  } catch (const std::exception& e) {
    std::throw_with_nested(std::runtime_error(CALL_INFO()));
  }
}

std::string TestConfiguration::getExecutableDir() {
  try {
    return executableDir.value_or("");
  } catch (const std::exception& e) {
    std::throw_with_nested(std::runtime_error(CALL_INFO()));
  }
}

std::pair<int, std::string> TestConfiguration::setExecutableFile(const char* value) {
  try {
    std::filesystem::path path = std::filesystem::path(std::string(value)).make_preferred();
    TestConfiguration::executableFile = path.string();
    TestConfiguration::executableDir = path.parent_path().string();
    return std::make_pair(EXIT_SUCCESS, "");
  } catch (const std::exception& e) {
    return std::make_pair(EXIT_FAILURE, TestUtils::toString(e));
  }
}
