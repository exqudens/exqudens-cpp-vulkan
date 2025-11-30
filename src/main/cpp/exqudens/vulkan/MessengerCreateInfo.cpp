#include "exqudens/vulkan/MessengerCreateInfo.hpp"

namespace exqudens::vulkan {

  MessengerCreateInfo& MessengerCreateInfo::setExceptionSeverity(const vk::DebugUtilsMessageSeverityFlagsEXT& value) {
    exceptionSeverity = value;
    return *this;
  }

  MessengerCreateInfo& MessengerCreateInfo::setOutSeverity(const vk::DebugUtilsMessageSeverityFlagsEXT& value) {
    outSeverity = value;
    return *this;
  }

  MessengerCreateInfo& MessengerCreateInfo::setToStringFunction(
      const std::function<std::string(
          vk::DebugUtilsMessageSeverityFlagsEXT,
          vk::DebugUtilsMessageTypeFlagsEXT,
          std::string
      )>& value
  ) {
    toStringFunction = value;
    return *this;
  }

}
