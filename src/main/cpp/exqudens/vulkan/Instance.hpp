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

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

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

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Context& contextReference();

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Instance& reference();

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Messenger& messengerReference();

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::DebugUtilsMessengerEXT& debugUtilsMessengerReference();

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

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& addEnabledLayerName(const char* val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& addEnabledLayerName(const std::string& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& setEnabledLayerNames(const std::vector<const char*>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& addEnabledExtensionName(const char* val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& addEnabledExtensionName(const std::string& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& setEnabledExtensionNames(const std::vector<const char*>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& setApplicationInfo(const vk::ApplicationInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& setCreateInfo(const vk::InstanceCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& setMessengerCreateInfo(const MessengerCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& setDebugUtilsMessengerCreateInfo(const vk::DebugUtilsMessengerCreateInfoEXT& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance::Builder& setOut(std::ostream& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Instance build();

  };

}
