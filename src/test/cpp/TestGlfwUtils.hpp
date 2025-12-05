#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "test_lib_export.hpp"

class TEST_LIB_EXPORT TestGlfwUtils {

    public:

        static std::vector<const char*> getRequiredInstanceExtensions();
};
