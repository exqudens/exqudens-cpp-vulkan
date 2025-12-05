#include <cstdint>
#include <string>
#include <stdexcept>
#include <filesystem>

#include "TestGlfwUtils.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

std::vector<const char*> TestGlfwUtils::getRequiredInstanceExtensions() {
    try {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = nullptr;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> result(glfwExtensions, glfwExtensions + glfwExtensionCount);
        return result;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

#undef CALL_INFO
