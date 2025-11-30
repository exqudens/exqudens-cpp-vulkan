#pragma once

#include <vector>
#include <set>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT QueueInfo {

    uint32_t familyIndex;
    std::vector<float> priorities;
    std::set<vk::QueueFlagBits> types;
    bool presentType;
    bool timestampSupported;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueueInfo& setFamilyIndex(const uint32_t& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueueInfo& addPriority(const float& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueueInfo& setPriorities(const std::vector<float>& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueueInfo& addType(const vk::QueueFlagBits& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueueInfo& setTypes(const std::set<vk::QueueFlagBits>& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueueInfo& setPresentType(const bool& val);

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES QueueInfo& setTimestampSupported(const bool& val);

  };

}
