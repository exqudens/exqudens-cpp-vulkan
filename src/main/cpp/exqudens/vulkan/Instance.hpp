#pragma once

#include <string>
#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"
#include "exqudens/vulkan/MessengerCreateInfo.hpp"
#include "exqudens/vulkan/Messenger.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Instance {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    std::shared_ptr<vk::raii::Context> context;
    std::vector<const char*> enabledLayerNames;
    std::vector<const char*> enabledExtensionNames;
    vk::ApplicationInfo applicationInfo;
    vk::InstanceCreateInfo createInfo;
    std::shared_ptr<vk::raii::Instance> value;
    MessengerCreateInfo messengerCreateInfo;
    std::shared_ptr<Messenger> messenger;
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
    std::shared_ptr<vk::raii::DebugUtilsMessengerEXT> debugUtilsMessenger;

    vk::raii::Context& contextReference();

    vk::raii::Instance& reference();

    Messenger& messengerReference();

    vk::raii::DebugUtilsMessengerEXT& debugUtilsMessengerReference();

  };

  class EXQUDENS_VULKAN_EXPORT Instance::Builder {

    private:

      std::vector<const char*> enabledLayerNames;
      std::vector<const char*> enabledExtensionNames;
      std::optional<vk::ApplicationInfo> applicationInfo;
      std::optional<vk::InstanceCreateInfo> createInfo;
      std::optional<MessengerCreateInfo> messengerCreateInfo;
      std::optional<vk::DebugUtilsMessengerCreateInfoEXT> debugUtilsMessengerCreateInfo;
      std::ostream* out;

    public:

      Instance::Builder& addEnabledLayerName(const char* val);

      Instance::Builder& addEnabledLayerName(const std::string& val);

      Instance::Builder& setEnabledLayerNames(const std::vector<const char*>& val);

      Instance::Builder& addEnabledExtensionName(const char* val);

      Instance::Builder& addEnabledExtensionName(const std::string& val);

      Instance::Builder& setEnabledExtensionNames(const std::vector<const char*>& val);

      Instance::Builder& setApplicationInfo(const vk::ApplicationInfo& val);

      Instance::Builder& setCreateInfo(const vk::InstanceCreateInfo& val);

      Instance::Builder& setMessengerCreateInfo(const MessengerCreateInfo& val);

      Instance::Builder& setDebugUtilsMessengerCreateInfo(const vk::DebugUtilsMessengerCreateInfoEXT& val);

      Instance::Builder& setOut(std::ostream& val);

      Instance build();

  };

}
