#include "exqudens/vulkan/QueueInfo.hpp"

namespace exqudens::vulkan {

  QueueInfo& QueueInfo::setFamilyIndex(const uint32_t& val) {
    familyIndex = val;
    return *this;
  }

  QueueInfo& QueueInfo::addPriority(const float& val) {
    priorities.emplace_back(val);
    return *this;
  }

  QueueInfo& QueueInfo::setPriorities(const std::vector<float>& val) {
    priorities = val;
    return *this;
  }

  QueueInfo& QueueInfo::addType(const vk::QueueFlagBits& val) {
    types.insert(val);
    return *this;
  }

  QueueInfo& QueueInfo::setTypes(const std::set<vk::QueueFlagBits>& val) {
    types = val;
    return *this;
  }

  QueueInfo& QueueInfo::setPresentType(const bool& val) {
    presentType = val;
    return *this;
  }

  QueueInfo& QueueInfo::setTimestampSupported(const bool& val) {
    timestampSupported = val;
    return *this;
  }

}
