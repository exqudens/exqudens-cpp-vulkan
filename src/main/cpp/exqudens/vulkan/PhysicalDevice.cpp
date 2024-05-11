#include <map>
#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/PhysicalDevice.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::PhysicalDevice& PhysicalDevice::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  PhysicalDevice::Builder& PhysicalDevice::Builder::setInstance(const std::weak_ptr<vk::raii::Instance>& val) {
    instance = val;
    return *this;
  }

  PhysicalDevice::Builder& PhysicalDevice::Builder::addEnabledExtensionName(const char* val) {
    enabledExtensionNames.emplace_back(val);
    return *this;
  }

  PhysicalDevice::Builder& PhysicalDevice::Builder::addEnabledExtensionName(const std::string& val) {
    enabledExtensionNames.emplace_back(val.c_str());
    return *this;
  }

  PhysicalDevice::Builder& PhysicalDevice::Builder::setEnabledExtensionNames(const std::vector<const char*>& val) {
    enabledExtensionNames = val;
    return *this;
  }

  PhysicalDevice::Builder& PhysicalDevice::Builder::setSurface(const vk::SurfaceKHR& val) {
    surface = val;
    return *this;
  }

  PhysicalDevice::Builder& PhysicalDevice::Builder::setSurface(const std::weak_ptr<vk::raii::SurfaceKHR>& val) {
    surface = *(*val.lock());
    return *this;
  }

  PhysicalDevice::Builder& PhysicalDevice::Builder::addQueuePriority(const float& val) {
    queuePriorities.emplace_back(val);
    return *this;
  }

  PhysicalDevice::Builder& PhysicalDevice::Builder::setQueuePriorities(const std::vector<float>& val) {
    queuePriorities = val;
    return *this;
  }

  PhysicalDevice::Builder& PhysicalDevice::Builder::setIsSuitableFunction(const std::function<bool(const vk::raii::PhysicalDevice&)>& val) {
    isSuitableFunction = val;
    return *this;
  }

  PhysicalDevice PhysicalDevice::Builder::build() {
    try {
      if (!isSuitableFunction) {
        throw std::runtime_error(CALL_INFO + ": 'isSuitableFunction' is not defined!");
      }

      PhysicalDevice target = {};
      target.enabledExtensionNames = enabledExtensionNames;
      target.queuePriorities = queuePriorities;
      std::vector<vk::raii::PhysicalDevice> values = vk::raii::PhysicalDevices(
          *instance.lock()
      );

      std::vector<QueueInfo> tmpQueueInfos = {};

      for (vk::raii::PhysicalDevice& physicalDevice : values) {

        bool suitable = isSuitableFunction(physicalDevice);

        if (suitable) {
          std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

          for (size_t i = 0; i < queueFamilyProperties.size(); i++) {

            QueueInfo queueInfo = QueueInfo()
                .setFamilyIndex(static_cast<uint32_t>(i))
                .setPriorities(target.queuePriorities);

            if (queueFamilyProperties[i].timestampValidBits > 0) {
              queueInfo.setTimestampSupported(true);
            }

            if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) {
              queueInfo.addType(vk::QueueFlagBits::eCompute);
            }

            if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer) {
              queueInfo.addType(vk::QueueFlagBits::eTransfer);
            }

            if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
              queueInfo.addType(vk::QueueFlagBits::eGraphics);
            }

            if (surface && physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface.value())) {
              queueInfo.setPresentType(true);
            }

            if (!queueInfo.types.empty() || queueInfo.presentType) {
              tmpQueueInfos.emplace_back(queueInfo);
            }
          }

          if (!tmpQueueInfos.empty()) {
            target.queueInfos = tmpQueueInfos;

            std::vector<vk::DeviceQueueCreateInfo> tmpComputeQueueCreateInfos = {};
            std::vector<vk::DeviceQueueCreateInfo> tmpTransferQueueCreateInfos = {};
            std::vector<vk::DeviceQueueCreateInfo> tmpGraphicsQueueCreateInfos = {};
            std::vector<vk::DeviceQueueCreateInfo> tmpPresentQueueCreateInfos = {};
            std::map<uint32_t, vk::DeviceQueueCreateInfo> queueCreateInfoMap = {};

            for (const QueueInfo& queueInfo : target.queueInfos) {
              vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo()
                  .setQueueFamilyIndex(queueInfo.familyIndex)
                  .setQueuePriorities(queueInfo.priorities);
              if (queueInfo.types.contains(vk::QueueFlagBits::eCompute)) {
                tmpComputeQueueCreateInfos.emplace_back(queueCreateInfo);
              }
              if (queueInfo.types.contains(vk::QueueFlagBits::eTransfer)) {
                tmpTransferQueueCreateInfos.emplace_back(queueCreateInfo);
              }
              if (queueInfo.types.contains(vk::QueueFlagBits::eGraphics)) {
                tmpGraphicsQueueCreateInfos.emplace_back(queueCreateInfo);
              }
              if (queueInfo.presentType) {
                tmpPresentQueueCreateInfos.emplace_back(queueCreateInfo);
              }
              queueCreateInfoMap.try_emplace(queueCreateInfo.queueFamilyIndex, queueCreateInfo);
            }

            target.computeQueueCreateInfos = tmpComputeQueueCreateInfos;
            target.transferQueueCreateInfos = tmpTransferQueueCreateInfos;
            target.graphicsQueueCreateInfos = tmpGraphicsQueueCreateInfos;
            target.presentQueueCreateInfos = tmpPresentQueueCreateInfos;
            std::vector<vk::DeviceQueueCreateInfo> tmpUniqueQueueCreateInfos = {};
            for (const auto& [k, v] : queueCreateInfoMap) {
              tmpUniqueQueueCreateInfos.emplace_back(v);
            }
            target.uniqueQueueCreateInfos = tmpUniqueQueueCreateInfos;
            target.value = std::make_shared<vk::raii::PhysicalDevice>(std::move(physicalDevice));
          }

          break;
        }

      }
      if (!target.value) {
        throw std::runtime_error(CALL_INFO + ": failed to create physical device!");
      }
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  PhysicalDevice::Builder PhysicalDevice::builder() {
    return {};
  }

}

#undef CALL_INFO
