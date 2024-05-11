#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Instance.hpp"
#include "exqudens/vulkan/Utility.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::Context& Instance::contextReference() {
    try {
      if (!context) {
        throw std::runtime_error(CALL_INFO + ": context is not initialized!");
      }
      return *context;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  vk::raii::Instance& Instance::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Messenger& Instance::messengerReference() {
    try {
      if (!messenger) {
        throw std::runtime_error(CALL_INFO + ": messenger is not initialized!");
      }
      return *messenger;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  vk::raii::DebugUtilsMessengerEXT& Instance::debugUtilsMessengerReference() {
    try {
      if (!debugUtilsMessenger) {
        throw std::runtime_error(CALL_INFO + ": debugUtilsMessenger is not initialized!");
      }
      return *debugUtilsMessenger;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Instance::Builder& Instance::Builder::addEnabledLayerName(const char* val) {
    enabledLayerNames.emplace_back(val);
    return *this;
  }

  Instance::Builder& Instance::Builder::addEnabledLayerName(const std::string& val) {
    enabledLayerNames.emplace_back(val.c_str());
    return *this;
  }

  Instance::Builder& Instance::Builder::setEnabledLayerNames(const std::vector<const char*>& val) {
    enabledLayerNames = val;
    return *this;
  }

  Instance::Builder& Instance::Builder::addEnabledExtensionName(const char* val) {
    enabledExtensionNames.emplace_back(val);
    return *this;
  }

  Instance::Builder& Instance::Builder::addEnabledExtensionName(const std::string& val) {
    enabledExtensionNames.emplace_back(val.c_str());
    return *this;
  }

  Instance::Builder& Instance::Builder::setEnabledExtensionNames(const std::vector<const char*>& val) {
    enabledExtensionNames = val;
    return *this;
  }

  Instance::Builder& Instance::Builder::setApplicationInfo(const vk::ApplicationInfo& val) {
    applicationInfo = val;
    return *this;
  }

  Instance::Builder& Instance::Builder::setCreateInfo(const vk::InstanceCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  Instance::Builder& Instance::Builder::setMessengerCreateInfo(const MessengerCreateInfo& val) {
    messengerCreateInfo = val;
    return *this;
  }

  Instance::Builder& Instance::Builder::setDebugUtilsMessengerCreateInfo(const vk::DebugUtilsMessengerCreateInfoEXT& val) {
    debugUtilsMessengerCreateInfo = val;
    return *this;
  }

  Instance::Builder& Instance::Builder::setOut(std::ostream& val) {
    out = &val;
    return *this;
  }

  Instance Instance::Builder::build() {
    try {
      Instance target = {};
      target.context = std::make_shared<vk::raii::Context>();
      target.enabledLayerNames = enabledLayerNames;
      target.enabledExtensionNames = enabledExtensionNames;
      target.applicationInfo = applicationInfo.value();
      target.createInfo = createInfo.value_or(vk::InstanceCreateInfo());
      target.createInfo.setPEnabledLayerNames(target.enabledLayerNames);
      target.createInfo.setPEnabledExtensionNames(target.enabledExtensionNames);
      target.createInfo.setPApplicationInfo(&target.applicationInfo);
      target.value = std::make_shared<vk::raii::Instance>(
          target.contextReference(),
          target.createInfo
      );
      target.messengerCreateInfo = messengerCreateInfo.value();
      if (!target.messengerCreateInfo.toStringFunction) {
        /*target.messengerCreateInfo.toStringFunction = std::bind(
            &Environment::toString,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        );*/
        /*target.messengerCreateInfo.toStringFunction = [this](auto&& o1, auto&& o2, auto&& o3) {
          return toString(
              std::forward<decltype(o1)>(o1),
              std::forward<decltype(o2)>(o2),
              std::forward<decltype(o3)>(o3)
          );
        };*/
        target.messengerCreateInfo.toStringFunction = &Utility::toString;
      }
      target.messenger = std::make_shared<Messenger>(
          *out,
          target.messengerCreateInfo
      );
      target.debugUtilsMessengerCreateInfo = debugUtilsMessengerCreateInfo.value();
      target.debugUtilsMessengerCreateInfo.setPUserData(&target.messengerReference());
      target.debugUtilsMessengerCreateInfo.setPfnUserCallback(&Messenger::callback);
      target.debugUtilsMessenger = std::make_shared<vk::raii::DebugUtilsMessengerEXT>(
          target.reference(),
          target.debugUtilsMessengerCreateInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Instance::Builder Instance::builder() {
    return {};
  }

}

#undef CALL_INFO
