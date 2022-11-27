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

    QueueInfo& setFamilyIndex(const uint32_t& val);

    QueueInfo& addPriority(const float& val);

    QueueInfo& setPriorities(const std::vector<float>& val);

    QueueInfo& addType(const vk::QueueFlagBits& val);

    QueueInfo& setTypes(const std::set<vk::QueueFlagBits>& val);

    QueueInfo& setPresentType(const bool& val);

    QueueInfo& setTimestampSupported(const bool& val);

  };

}
