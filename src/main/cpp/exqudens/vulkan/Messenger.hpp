#pragma once

#include <string>
#include <optional>
#include <functional>
#include <ostream>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"
#include "exqudens/vulkan/MessengerCreateInfo.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Messenger {

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static VkBool32 callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT cSeverity,
        VkDebugUtilsMessageTypeFlagsEXT cType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* data
    );

    std::optional<vk::DebugUtilsMessageSeverityFlagsEXT> exceptionSeverity;
    std::optional<vk::DebugUtilsMessageSeverityFlagsEXT> outSeverity;
    std::function<std::string(
        vk::DebugUtilsMessageSeverityFlagsEXT,
        vk::DebugUtilsMessageTypeFlagsEXT,
        std::string
    )> toStringFunction;
    std::ostream* value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Messenger(
        std::ostream& out,
        const MessengerCreateInfo& createInfo
    );

  };

}
