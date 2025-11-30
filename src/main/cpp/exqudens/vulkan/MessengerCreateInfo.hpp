#pragma once

#include <string>
#include <optional>
#include <functional>
#include <ostream>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT MessengerCreateInfo {

    std::optional<vk::DebugUtilsMessageSeverityFlagsEXT> exceptionSeverity;
    std::optional<vk::DebugUtilsMessageSeverityFlagsEXT> outSeverity;
    std::function<std::string(
        vk::DebugUtilsMessageSeverityFlagsEXT,
        vk::DebugUtilsMessageTypeFlagsEXT,
        std::string
    )> toStringFunction;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES MessengerCreateInfo& setExceptionSeverity(const vk::DebugUtilsMessageSeverityFlagsEXT& value);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES MessengerCreateInfo& setOutSeverity(const vk::DebugUtilsMessageSeverityFlagsEXT& value);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES MessengerCreateInfo& setToStringFunction(
        const std::function<std::string(
            vk::DebugUtilsMessageSeverityFlagsEXT,
            vk::DebugUtilsMessageTypeFlagsEXT,
            std::string
        )>& value
    );

  };

}
