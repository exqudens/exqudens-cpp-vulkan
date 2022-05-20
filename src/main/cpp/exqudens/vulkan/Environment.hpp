#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <optional>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <limits>
#include <utility>
#include <algorithm>
#include <ostream>

#include "exqudens/vulkan/Macros.hpp"
#include "exqudens/vulkan/Context.hpp"
#include "exqudens/vulkan/Instance.hpp"
#include "exqudens/vulkan/Messenger.hpp"
#include "exqudens/vulkan/DebugUtilsMessenger.hpp"
#include "exqudens/vulkan/Surface.hpp"
#include "exqudens/vulkan/PhysicalDevice.hpp"
#include "exqudens/vulkan/Device.hpp"
#include "exqudens/vulkan/SwapChain.hpp"

namespace exqudens::vulkan {

  class Environment {

    protected:

      unsigned int contextId = 1;
      unsigned int instanceId = 1;
      unsigned int messengerId = 1;
      unsigned int debugUtilsMessengerId = 1;
      unsigned int surfaceId = 1;
      unsigned int physicalDeviceId = 1;
      unsigned int deviceId = 1;
      unsigned int swapChainId = 1;

      std::vector<vk::raii::PhysicalDevice> physicalDevices = {};

      std::map<unsigned int, std::shared_ptr<Context>> contextMap = {};
      std::map<unsigned int, std::shared_ptr<Instance>> instanceMap = {};
      std::map<unsigned int, std::shared_ptr<Messenger>> messengerMap = {};
      std::map<unsigned int, std::shared_ptr<DebugUtilsMessenger>> debugUtilsMessengerMap = {};
      std::map<unsigned int, std::shared_ptr<Surface>> surfaceMap = {};
      std::map<unsigned int, std::shared_ptr<PhysicalDevice>> physicalDeviceMap = {};
      std::map<unsigned int, std::shared_ptr<Device>> deviceMap = {};
      std::map<unsigned int, std::shared_ptr<SwapChain>> swapChainMap = {};

    public:

      virtual std::shared_ptr<Context> createContext(
          const ContextCreateInfo& createInfo
      ) {
        try {
          for (auto const& [key, value] : createInfo.environmentVariables) {
            setEnvironmentVariable(std::string(key), std::string(value));
          }

          auto* value = new Context;
          value->id = contextId++;
          value->createInfo = createInfo;

          auto* object = new vk::raii::Context;

          value->value = std::shared_ptr<vk::raii::Context>(object);

          return contextMap[value->id] = std::shared_ptr<Context>(value);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual std::shared_ptr<Instance> createInstance(
          Context& context,
          const vk::ApplicationInfo& applicationInfo,
          const vk::InstanceCreateInfo& createInfo
      ) {
        try {
          auto* value = new Instance;
          value->id = instanceId++;
          value->applicationInfo = applicationInfo;
          value->createInfo = createInfo;
          value->createInfo.pApplicationInfo = &value->applicationInfo;
          auto* object = new vk::raii::Instance(
              *context.value,
              value->createInfo
          );
          value->value = std::shared_ptr<vk::raii::Instance>(object);
          return instanceMap[value->id] = std::shared_ptr<Instance>(value);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual std::shared_ptr<Messenger> createMessenger(
          std::ostream& out,
          const std::optional<vk::DebugUtilsMessageSeverityFlagsEXT>& exceptionSeverity,
          const std::optional<vk::DebugUtilsMessageSeverityFlagsEXT>& outSeverity,
          const std::function<std::string(
              vk::DebugUtilsMessageSeverityFlagsEXT,
              vk::DebugUtilsMessageTypeFlagsEXT,
              std::string
          )>& toStringFunction
      ) {
        try {
          auto* value = new Messenger;
          value->id = messengerId++;
          value->exceptionSeverity = exceptionSeverity;
          value->outSeverity = outSeverity;
          if (toStringFunction) {
            value->toStringFunction = toStringFunction;
          } else {
            /*value->toStringFunction = std::bind(
                &Environment::toString,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
            );*/
            value->toStringFunction = [this](auto&& o1, auto&& o2, auto&& o3) {
              return toString(
                  std::forward<decltype(o1)>(o1),
                  std::forward<decltype(o2)>(o2),
                  std::forward<decltype(o3)>(o3)
              );
            };
          }
          value->out = &out;
          return messengerMap[value->id] = std::shared_ptr<Messenger>(value);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual std::shared_ptr<DebugUtilsMessenger> createDebugUtilsMessenger(
          Instance& instance,
          Messenger& messenger,
          const vk::DebugUtilsMessengerCreateInfoEXT& createInfo
      ) {
        try {
          auto* value = new DebugUtilsMessenger;
          value->id = debugUtilsMessengerId++;
          value->createInfo = createInfo;
          auto* object = new vk::raii::DebugUtilsMessengerEXT(
              *instance.value,
              createInfo
          );
          value->value = std::shared_ptr<vk::raii::DebugUtilsMessengerEXT>(object);
          return debugUtilsMessengerMap[value->id] = std::shared_ptr<DebugUtilsMessenger>(value);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual std::shared_ptr<Surface> createSurface(
          Instance& instance,
          VkSurfaceKHR& vkSurface
      ) {
        try {
          auto* value = new Surface;
          value->id = surfaceId++;
          auto* object = new vk::raii::SurfaceKHR(*instance.value, vkSurface);
          value->value = std::shared_ptr<vk::raii::SurfaceKHR>(object);
          return surfaceMap[value->id] = std::shared_ptr<Surface>(value);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual std::shared_ptr<PhysicalDevice> createPhysicalDevice(
          Instance& instance,
          const std::vector<const char*>& enabledDeviceExtensionNames,
          const vk::PhysicalDeviceFeatures& features,
          const float& queuePriorities,
          const std::vector<vk::QueueFlagBits>& queueTypes,
          const std::shared_ptr<Surface>& surface
      ) {
        try {
          auto* value = new PhysicalDevice;
          value->id = physicalDeviceId++;
          value->queuePriorities = queuePriorities;
          value->features = features;
          physicalDevices = vk::raii::PhysicalDevices(*instance.value);
          for (vk::raii::PhysicalDevice& physicalDevice : physicalDevices) {

            bool queueFamilyIndicesAdequate = true;
            bool deviceExtensionAdequate = true;
            bool swapChainAdequate = true;
            bool anisotropyAdequate = true;

            std::vector<vk::DeviceQueueCreateInfo> computeQueueCreateInfos = {};
            std::vector<vk::DeviceQueueCreateInfo> transferQueueCreateInfos = {};
            std::vector<vk::DeviceQueueCreateInfo> graphicsQueueCreateInfos = {};
            std::vector<vk::DeviceQueueCreateInfo> presentQueueCreateInfos = {};
            std::map<uint32_t, vk::DeviceQueueCreateInfo> queueCreateInfoMap = {};

            std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
            for (const vk::QueueFlagBits& queueType : queueTypes) {
              std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {};
              for (std::size_t i = 0; i < queueFamilyProperties.size(); i++) {
                if (queueFamilyProperties[i].queueFlags & queueType) {
                  vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo()
                      .setQueueFamilyIndex(static_cast<uint32_t>(i))
                      .setQueueCount(queueFamilyProperties[i].queueCount)
                      .setFlags({})
                      .setQueuePriorities(value->queuePriorities);
                  queueCreateInfos.emplace_back(queueCreateInfo);
                  queueCreateInfoMap.try_emplace(queueCreateInfo.queueFamilyIndex, queueCreateInfo);
                }
              }
              if (queueCreateInfos.empty()) {
                queueFamilyIndicesAdequate = false;
                break;
              } else {
                if (vk::QueueFlagBits::eCompute == queueType) {
                  computeQueueCreateInfos = queueCreateInfos;
                } else if (vk::QueueFlagBits::eTransfer == queueType) {
                  transferQueueCreateInfos = queueCreateInfos;
                } else if (vk::QueueFlagBits::eGraphics == queueType) {
                  graphicsQueueCreateInfos = queueCreateInfos;
                }
              }
            }
            if (surface) {
              std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {};
              for (std::size_t i = 0; i < queueFamilyProperties.size(); i++) {
                if (physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *(*surface->value))) {
                  vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo()
                      .setQueueFamilyIndex(static_cast<uint32_t>(i))
                      .setQueueCount(queueFamilyProperties[i].queueCount)
                      .setFlags({})
                      .setQueuePriorities(value->queuePriorities);
                  queueCreateInfos.emplace_back(queueCreateInfo);
                  queueCreateInfoMap.try_emplace(queueCreateInfo.queueFamilyIndex, queueCreateInfo);
                }
              }
              if (queueCreateInfos.empty()) {
                presentQueueCreateInfos = queueCreateInfos;
                queueFamilyIndicesAdequate = false;
              }
            }
            if (!queueFamilyIndicesAdequate) {
              break;
            }

            std::vector<vk::ExtensionProperties> extensionProperties = physicalDevice.enumerateDeviceExtensionProperties(nullptr);
            std::set<std::string> requiredExtensions(enabledDeviceExtensionNames.begin(), enabledDeviceExtensionNames.end());
            for (const auto& extension : extensionProperties) {
              std::string name = std::string(extension.extensionName.data());
              requiredExtensions.erase(name);
            }
            deviceExtensionAdequate = requiredExtensions.empty();
            if (!deviceExtensionAdequate) {
              break;
            }

            if (surface) {
              std::vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR(*(*surface->value));
              std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(*(*surface->value));
              swapChainAdequate = !formats.empty() && !presentModes.empty();
            }
            if (!swapChainAdequate) {
              break;
            }

            if (features.samplerAnisotropy) {
              vk::PhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.getFeatures();
              anisotropyAdequate = physicalDeviceFeatures.samplerAnisotropy;
            }
            if (!anisotropyAdequate) {
              break;
            }

            if (
                queueFamilyIndicesAdequate
                && deviceExtensionAdequate
                && swapChainAdequate
                && anisotropyAdequate
            ) {
              value->computeQueueCreateInfos = computeQueueCreateInfos;
              value->transferQueueCreateInfos = transferQueueCreateInfos;
              value->graphicsQueueCreateInfos = graphicsQueueCreateInfos;
              value->presentQueueCreateInfos = presentQueueCreateInfos;
              std::vector<vk::DeviceQueueCreateInfo> uniqueQueueCreateInfos;
              for (const auto& [k, v] : queueCreateInfoMap) {
                uniqueQueueCreateInfos.emplace_back(v);
              }
              value->uniqueQueueCreateInfos = uniqueQueueCreateInfos;
              value->value = &physicalDevice;
              break;
            }
          }
          return physicalDeviceMap[value->id] = std::shared_ptr<PhysicalDevice>(value);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual std::shared_ptr<Device> createDevice(
          PhysicalDevice& physicalDevice,
          const vk::DeviceCreateInfo& createInfo
      ) {
        try {
          auto* value = new Device;
          value->id = deviceId++;
          value->createInfo = createInfo;
          auto* object = new vk::raii::Device(
              *physicalDevice.value,
              value->createInfo
          );
          value->value = std::shared_ptr<vk::raii::Device>(object);
          return deviceMap[value->id] = std::shared_ptr<Device>(value);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual std::shared_ptr<SwapChain> createSwapChain(
          Device& device,
          const vk::SwapchainCreateInfoKHR& createInfo
      ) {
        try {
          auto* value = new SwapChain;
          value->id = swapChainId++;
          value->createInfo = createInfo;
          auto* object = new vk::raii::SwapchainKHR(
              *device.value,
              value->createInfo
          );
          value->value = std::shared_ptr<vk::raii::SwapchainKHR>(object);
          return swapChainMap[value->id] = std::shared_ptr<SwapChain>(value);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

    public:

      virtual void setEnvironmentVariable(const std::string& name, const std::string& value) {
        try {
#if defined(_WINDOWS)
          _putenv_s(name.c_str(), value.c_str());
#endif
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual std::optional<std::string> getEnvironmentVariable(const std::string& name) {
        try {
          std::optional<std::string> value;
#if defined(_WINDOWS)
          char* buffer;
          size_t size;
          errno_t error = _dupenv_s(&buffer, &size, name.c_str());
          if (error) {
            return value;
          }
          if (buffer != nullptr) {
            value.emplace(std::string(buffer));
          }
#endif
          return value;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual std::string toString(
          vk::DebugUtilsMessageSeverityFlagsEXT severity,
          vk::DebugUtilsMessageTypeFlagsEXT type,
          const std::string& message
      ) {
        try {
          return vk::to_string(severity) + " " + vk::to_string(type) + ": " + message;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      virtual vk::SwapchainCreateInfoKHR swapChainCreateInfo(
          PhysicalDevice& physicalDevice,
          Surface& surface,
          uint32_t& width,
          uint32_t& height
      ) {
        try {
          std::optional<vk::SurfaceFormatKHR> surfaceFormat;
          std::optional<vk::PresentModeKHR> surfacePresentMode;
          std::optional<vk::SurfaceCapabilitiesKHR> surfaceCapabilities;
          std::optional<vk::Extent2D> surfaceExtent;
          std::optional<vk::SurfaceTransformFlagBitsKHR> surfaceTransform;
          std::optional<vk::CompositeAlphaFlagBitsKHR> surfaceCompositeAlpha;

          std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.value->getSurfaceFormatsKHR(*(*surface.value));
          if (surfaceFormats.size() == 1 && surfaceFormats.front() == vk::Format::eUndefined) {
            surfaceFormat = surfaceFormats.front();
          } else {
            for (const vk::SurfaceFormatKHR& f : surfaceFormats) {
              if (vk::Format::eB8G8R8A8Srgb == f.format && vk::ColorSpaceKHR::eSrgbNonlinear == f.colorSpace) {
                surfaceFormat = f;
                break;
              }
            }
          }

          std::vector<vk::PresentModeKHR> surfacePresentModes = physicalDevice.value->getSurfacePresentModesKHR(*(*surface.value));
          surfacePresentMode = vk::PresentModeKHR::eFifo;
          for (const vk::PresentModeKHR& p : surfacePresentModes) {
            if (vk::PresentModeKHR::eMailbox == p) {
              surfacePresentMode = p;
              break;
            }
          }

          surfaceCapabilities = physicalDevice.value->getSurfaceCapabilitiesKHR(*(*surface.value));

          if (surfaceCapabilities.value().currentExtent.width == std::numeric_limits<uint32_t>::max()) {
            surfaceExtent = vk::Extent2D()
                .setWidth(std::clamp(width, surfaceCapabilities.value().minImageExtent.width, surfaceCapabilities.value().maxImageExtent.width))
                .setHeight(std::clamp(height, surfaceCapabilities.value().minImageExtent.height, surfaceCapabilities.value().maxImageExtent.height));
          } else {
            surfaceExtent = surfaceCapabilities.value().currentExtent;
          }

          if (surfaceCapabilities.value().supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
            surfaceTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
          } else {
            surfaceTransform = surfaceCapabilities.value().currentTransform;
          }

          if (surfaceCapabilities.value().supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
            surfaceCompositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
          } else if (surfaceCapabilities.value().supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
            surfaceCompositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePostMultiplied;
          } else if (surfaceCapabilities.value().supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) {
            surfaceCompositeAlpha = vk::CompositeAlphaFlagBitsKHR::eInherit;
          } else {
            surfaceCompositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
          }

          return vk::SwapchainCreateInfoKHR()
              .setFlags({})
              .setSurface(*(*surface.value))
              .setMinImageCount(surfaceCapabilities.value().minImageCount)
              .setImageFormat(surfaceFormat.value().format)
              .setImageColorSpace(surfaceFormat.value().colorSpace)
              .setImageExtent(surfaceExtent.value())
              .setImageArrayLayers(1)
              .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
              .setImageSharingMode(vk::SharingMode::eExclusive)
              .setQueueFamilyIndices({})
              .setPreTransform(surfaceTransform.value())
              .setCompositeAlpha(surfaceCompositeAlpha.value())
              .setPresentMode(surfacePresentMode.value())
              .setClipped(true)
              .setOldSwapchain({});
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

}
