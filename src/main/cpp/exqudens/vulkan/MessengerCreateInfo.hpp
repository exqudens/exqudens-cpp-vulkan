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

    MessengerCreateInfo& setExceptionSeverity(const vk::DebugUtilsMessageSeverityFlagsEXT& value);

    MessengerCreateInfo& setOutSeverity(const vk::DebugUtilsMessageSeverityFlagsEXT& value);

    MessengerCreateInfo& setToStringFunction(
        const std::function<std::string(
            vk::DebugUtilsMessageSeverityFlagsEXT,
            vk::DebugUtilsMessageTypeFlagsEXT,
            std::string
        )>& value
    );

  };

}
