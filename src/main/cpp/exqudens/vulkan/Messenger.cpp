#include <iostream>
#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Messenger.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  VkBool32 Messenger::callback(
      VkDebugUtilsMessageSeverityFlagBitsEXT cSeverity,
      VkDebugUtilsMessageTypeFlagsEXT cType,
      const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
      void* data
  ) {
    Messenger* messenger = nullptr;
    std::string formatted = "<UNDEFINED>";
    try {
      auto severity = vk::DebugUtilsMessageSeverityFlagsEXT(cSeverity);
      auto type = vk::DebugUtilsMessageTypeFlagsEXT(cType);
      std::string message(callbackData->pMessage);
      formatted = vk::to_string(severity) + " " + vk::to_string(type) + ": " + message;

      if (data != nullptr) {
        try {
          messenger = reinterpret_cast<Messenger*>(data);
        } catch (...) {
        }
        if (messenger != nullptr) {
          if (messenger->toStringFunction) {
            formatted = messenger->toStringFunction(severity, type, message);
          }
          if (
              messenger->exceptionSeverity.has_value()
              && severity >= messenger->exceptionSeverity.value()
              ) {
            throw std::runtime_error(formatted);
          }
          if (
              messenger->outSeverity.has_value()
              && severity >= messenger->outSeverity.value()
              ) {
            if (messenger->value != nullptr) {
              *messenger->value << formatted << std::endl;
              return VK_FALSE;
            } else {
              std::cout << formatted << std::endl;
              return VK_FALSE;
            }
          }
        }
      }

      return VK_FALSE;
    } catch (const std::exception& e) {
      if (messenger != nullptr && messenger->value != nullptr) {
        *messenger->value << formatted << std::endl;
      } else {
        std::cout << formatted << std::endl;
      }
      std::throw_with_nested(std::runtime_error(CALL_INFO + ": " + e.what()));
    } catch (...) {
      if (messenger != nullptr && messenger->value != nullptr) {
        *messenger->value << formatted << std::endl;
      } else {
        std::cout << formatted << std::endl;
      }
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Messenger::Messenger(
      std::ostream& out,
      const exqudens::vulkan::MessengerCreateInfo& createInfo
  ):
      exceptionSeverity(createInfo.exceptionSeverity),
      outSeverity(createInfo.outSeverity),
      toStringFunction(createInfo.toStringFunction),
      value(&out)
  {
  }

}

#undef CALL_INFO
