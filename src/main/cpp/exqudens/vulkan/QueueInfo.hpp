#pragma once

#include <vector>
#include <set>

#include <vulkan/vulkan_raii.hpp>

namespace exqudens::vulkan {

  struct QueueInfo {

    uint32_t familyIndex;
    std::vector<float> priorities;
    std::set<vk::QueueFlagBits> types;
    bool presentType;
    bool timestampSupported;

    QueueInfo& setFamilyIndex(const uint32_t& val) {
      familyIndex = val;
      return *this;
    }

    QueueInfo& addPriority(const float& val) {
      priorities.emplace_back(val);
      return *this;
    }

    QueueInfo& setPriorities(const std::vector<float>& val) {
      priorities = val;
      return *this;
    }

    QueueInfo& addType(const vk::QueueFlagBits& val) {
      types.insert(val);
      return *this;
    }

    QueueInfo& setTypes(const std::set<vk::QueueFlagBits>& val) {
      types = val;
      return *this;
    }

    QueueInfo& setPresentType(const bool& val) {
      presentType = val;
      return *this;
    }

    QueueInfo& setTimestampSupported(const bool& val) {
      timestampSupported = val;
      return *this;
    }

  };

}
