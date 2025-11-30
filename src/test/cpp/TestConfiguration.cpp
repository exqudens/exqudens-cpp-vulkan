#include <filesystem>
#include <stdexcept>

#include "TestConfiguration.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

std::string TestConfiguration::getExecutableFile() {
  try {
    return executableFile.value();
  } catch (const std::exception& e) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestConfiguration::getExecutableDir() {
  try {
    return executableDir.value();
  } catch (const std::exception& e) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestConfiguration::setExecutableFile(const std::string& value) {
  try {
    if (value.empty()) {
      throw std::invalid_argument("'value' is empty!");
    }
    std::filesystem::path path = std::filesystem::path(std::string(value)).make_preferred();
    executableFile = path.string();
    executableDir = path.parent_path().string();
  } catch (const std::exception& e) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

#undef CALL_INFO
